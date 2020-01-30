#include "renderer.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

void printBoundingVolume(BoundingVolume *bv) {
	for (int i = 0; i < bv->nChildren; i++) {
		printBoundingVolume(bv->children + i);
	}
//	printf("-----\n");
	//	for (int i = 0; i < bv->nTriangles; i++) {
	static char c = 'A';
	printf("%c = [%.2f, %.2f, %.2f] \n", c, bv->low.x, bv->low.y, bv->low.z);
	c++;
	printf("%c = [%.2f, %.2f, %.2f] \n", c, bv->high.x, bv->high.y, bv->high.z);
	c++;
	printf("drawBox(%c, %c) \n", c - 2, c - 1);
	//	}
//	printf("-----\n");
}

void rendererInit(Renderer *renderer) {
	for (int i = 0; i < renderer->scene->nTriangles; i++) {
		Triangle3D *t = &renderer->scene->triangles[i];
		Vector3D v1;
		sub(t->p1, t->p3, &v1);
		Vector3D v2;
		sub(t->p1, t->p2, &v2);
		t->plane = (Plane3D *)malloc(sizeof(Plane3D));
		t->plane->v = (Vector3D *)malloc(sizeof(Vector3D));
		cross(&v1, &v2, t->plane->v);
		norm(t->plane->v, t->plane->v);
		t->plane->d = -dot(t->p1, t->plane->v);

		printf("init triangle : N (%.2f, %.2f, %.2f) d %.2f \n", t->plane->v->x,
					 t->plane->v->y, t->plane->v->z, t->plane->d);
		printf("  color : C (%f, %f, %f) \n", t->colorR, t->colorG, t->colorB);
		printf("  refl  : C (%.2f, %.2f, %.2f) \n", t->k_d, t->k_e, t->k_s);
	}
	BoundingVolume *bv = malloc(sizeof(BoundingVolume));
	// convert scene triangles to double pointers to triangles
	Triangle3D **triangles =
			malloc(sizeof(Triangle3D *) * (size_t)renderer->scene->nTriangles);
	for (int i = 0; i < renderer->scene->nTriangles; i++) {
		triangles[i] = renderer->scene->triangles + i;
	}
	bv->triangles = triangles;
	bv->nTriangles = renderer->scene->nTriangles;
	constructBoundingVolumes(bv);
	printf("%d \n", bv->nChildren);
	printf("%d \n", bv->children->nChildren);
	printBoundingVolume(bv);
	//	printf("low  (%.2f, %.2f, %.2f) \n", bv->low.x, bv->low.y, bv->low.z);
	//	printf("high (%.2f, %.2f, %.2f) \n", bv->high.x, bv->high.y, bv->high.z);
}

void *rayTraceSegment(void *pSegment) {

	RendererSegment *rSegment = (RendererSegment *)pSegment;

	Ray3D *ray;
	unsigned char *p = rSegment->screen;

	for (int y = rSegment->startLine; y < rSegment->stopLine; y++) {
		for (int x = 0; x < rSegment->width; x++) {
			ray = constructRayThroughPixel(rSegment->renderer->camera, x, y,
																		 rSegment->width, rSegment->height);

			float rgb[3];
			traceRay(rSegment->renderer->camera, rSegment->renderer->scene, ray, 3,
							 rgb);

			if (rgb[0] > 1) {
				*p++ = 255;
			} else if (rgb[0] < 0) {
				*p++ = 0;
			} else {
				*p++ = (unsigned char)(rgb[0] * 255);
			}
			if (rgb[1] > 1) {
				*p++ = 255;
			} else if (rgb[1] < 0) {
				*p++ = 0;
			} else {
				*p++ = (unsigned char)(rgb[1] * 255);
			}
			if (rgb[2] > 1) {
				*p++ = 255;
			} else if (rgb[2] < 0) {
				*p++ = 0;
			} else {
				*p++ = (unsigned char)(rgb[2] * 255);
			}
		}
	}
	return NULL;
}

void rayTrace(Renderer *renderer, unsigned char *screen, int width,
							int height) {

	pthread_t *threads =
			(pthread_t *)malloc(sizeof(pthread_t) * (size_t)renderer->nThreads);
	RendererSegment *rSegments = (RendererSegment *)malloc(
			sizeof(RendererSegment) * (size_t)renderer->nThreads);
	float lines = (float)height / renderer->nThreads;

	for (int i = 0; i < renderer->nThreads; i++, rSegments++) {
		rSegments->renderer = renderer;
		rSegments->screen = screen + 3 * width * (int)(i * lines + .5f);
		rSegments->width = width;
		rSegments->height = height;
		rSegments->startLine = (int)(i * lines + .5f);
		rSegments->stopLine = (int)((i + 1) * lines + .5f);
		pthread_create(threads + i, NULL, rayTraceSegment, (void *)rSegments);
	}
	for (int i = 0; i < renderer->nThreads; i++) {
		pthread_join(threads[i], NULL);
	}
}

// Constructs a ray that originates at the camera position and shoots through
// the given pixel (x,y)
Ray3D *constructRayThroughPixel(Camera *camera, int x, int y, int imageWidth,
																int imageHeight) {
	Ray3D *ray = (Ray3D *)malloc(sizeof(Ray3D));
	float aspectRatio = (float)imageWidth / imageHeight;
	float scale = tanf(getRad(camera->fov / 2));

	// TODO: move all the constants to the loop in rayTrace
	float Px = (2 * (x + 0.5f) / imageWidth - 1) * scale * aspectRatio;
	float Py = (1 - 2 * (y + 0.5f) / imageHeight) * scale;

	Vector3D origin = {0, 0, 0};
	origin = applyTransformation(origin, camera->cameraToWorld);
	Vector3D pWorld = {Px, Py, -1};
	pWorld = applyTransformation(pWorld, camera->cameraToWorld);

	ray->p = (Vector3D *)malloc(sizeof(Vector3D));
	ray->p->x = origin.x;
	ray->p->y = origin.y;
	ray->p->z = origin.z;

	ray->v = (Vector3D *)malloc(sizeof(Vector3D));
	sub(&pWorld, &origin, ray->v);
	norm(ray->v, ray->v);

	return ray;
}

// recursive ray tracing for depth levels
// TODO: prevent excess dynamic allocation for pixel
float *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int depth,
								float *rgb) {

	Intersection3D *intersection = findIntersection(scene, ray);

	if (intersection != 0) {
		getColor(camera, scene, intersection, depth, rgb);
		free(intersection);
	} else {
		rgb[0] = scene->bkgR; // background light
		rgb[1] = scene->bkgG; // background light
		rgb[2] = scene->bkgB; // background light
	}
	return rgb;
}

// generate bounding volumes
// in: scene
// create node around all triangles
// if width > height
//   find triangle number n/2 x-wise
//   child 1 = triangles 0 to n/2
//   child 2 = triangles n/2 to n
//   recurse into children
// else
//   find triangle number n/2 y-wise
//   child 1 = triangles 0 to n/2
//   child 2 = triangles n/2 to n
//   recurse into children
BoundingVolume *constructBoundingVolumes(BoundingVolume *bv) {

	// determine own bounding volume points
	{
		Triangle3D *cur;
		bv->low = ORIGIN_3D;
		bv->high = ORIGIN_3D;
		for (int i = 0; i < bv->nTriangles; i++) {
			cur = bv->triangles[i];
			bv->low.x =
					fminf(bv->low.x, fminf(fminf(cur->p1->x, cur->p2->x), cur->p3->x));
			bv->low.y =
					fminf(bv->low.y, fminf(fminf(cur->p1->y, cur->p2->y), cur->p3->y));
			bv->low.z =
					fminf(bv->low.z, fminf(fminf(cur->p1->z, cur->p2->z), cur->p3->z));
			bv->high.x =
					fmaxf(bv->high.x, fmaxf(fmaxf(cur->p1->x, cur->p2->x), cur->p3->x));
			bv->high.y =
					fmaxf(bv->high.y, fmaxf(fmaxf(cur->p1->y, cur->p2->y), cur->p3->y));
			bv->high.z =
					fmaxf(bv->high.z, fmaxf(fmaxf(cur->p1->z, cur->p2->z), cur->p3->z));
		}
	}
	bv->nChildren = 0;

	// end at 2 triangles per volume
	if (bv->nTriangles > 2) {
		int nLow = bv->nTriangles / 2;
		Triangle3D **low =
				(Triangle3D **)malloc(sizeof(Triangle3D *) * (size_t)nLow);
		int nHigh = (int)((bv->nTriangles + .5f) / 2);
		Triangle3D **high = (Triangle3D **)malloc( //
				sizeof(Triangle3D *) * (size_t)nHigh);

		Vector3D bvDims;
		sub(&bv->high, &bv->low, &bvDims);
		printf("dims (%.2f, %.2f, %.2f) \n", bvDims.x, bvDims.y, bvDims.z);

		if (bvDims.x >= bvDims.y && bvDims.x >= bvDims.z) { // split along x
			printf("splitting along x \n");
			// sort using x
			// only sort the first half because we simply need to split it into two
			for (int i = 0; i < bv->nTriangles / 2; i++) {
				float min = INFINITY;
				float max = -INFINITY;
				int minInd = 0, maxInd = 0;
				for (int j = 2 * i; j < bv->nTriangles; j++) {
					// centroid c in axis (if you divide by 3)
					float c = (*(bv->triangles + j))->p1->x + (*(bv->triangles + j))->p2->x +
										(*(bv->triangles + j))->p3->x;
					printf("%d %d %.2f \n", i, j, c);
					if (c > max) {
						max = c;
						maxInd = j;
					}
					if (c < min) {
						min = c;
						minInd = j;
					}
				}
				printf("low (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p1->x, (*(bv->triangles + minInd))->p1->y, (*(bv->triangles + minInd))->p1->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p2->x, (*(bv->triangles + minInd))->p2->y, (*(bv->triangles + minInd))->p2->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p3->x, (*(bv->triangles + minInd))->p3->y, (*(bv->triangles + minInd))->p3->z);
				printf("max (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p1->x, (*(bv->triangles + maxInd))->p1->y, (*(bv->triangles + maxInd))->p1->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p2->x, (*(bv->triangles + maxInd))->p2->y, (*(bv->triangles + maxInd))->p2->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p3->x, (*(bv->triangles + maxInd))->p3->y, (*(bv->triangles + maxInd))->p3->z);
				*(low + i) = *(bv->triangles + minInd);
				*(high + i) = *(bv->triangles + maxInd);
				// swap to get rid of those elements in the bucket
//				printf("swap %d into %d \n", 2 * i, minInd);
//				printf("swap %d into %d \n", 2 * i + 1, maxInd);

//				float a = (*(bv->triangles + minInd))->p2->x;
//				float b = (*(bv->triangles + 2 * i))->p2->x;
//				printf("%2.f %.2f \n", a, b);

				*(bv->triangles + minInd) = *(bv->triangles + 2 * i);
				*(bv->triangles + maxInd) = *(bv->triangles + 2 * i + 1);

//				a = (*(bv->triangles + minInd))->p2->x;
//				b = (*(bv->triangles + 2 * i))->p2->x;
//				printf("%2.f %.2f \n", a, b);
			}
			for (int i = 0; i < nLow; i++) {
				printf("lows (%.2f %.2f %.2f) \n", low[i]->p1->x, low[i]->p1->y, low[i]->p1->z);
				printf("    (%.2f %.2f %.2f) \n", low[i]->p2->x, low[i]->p2->y, low[i]->p2->z);
				printf("    (%.2f %.2f %.2f) \n", low[i]->p3->x, low[i]->p3->y, low[i]->p3->z);
			}
		} else if (bvDims.y >= bvDims.x && bvDims.y >= bvDims.z) { // split along y
			printf("splitting along y \n");
			// sort using y distance
			// only sort the first half because we simply need to split it into two
			for (int i = 0; i < bv->nTriangles / 2; i++) {
				float min = INFINITY;
				float max = -INFINITY;
				int minInd = 0, maxInd = 0;
				for (int j = 2 * i; j < bv->nTriangles; j++) {
					// centroid c in axis (if you divide by 3)
					float c = (*(bv->triangles + j))->p1->y + (*(bv->triangles + j))->p2->y +
										(*(bv->triangles + j))->p3->y;
					printf("%d %d %.2f \n", i, j, c);
					if (c > max) {
						max = c;
						maxInd = j;
					}
					if (c < min) {
						min = c;
						minInd = j;
					}
				}
				printf("low (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p1->x, (*(bv->triangles + minInd))->p1->y, (*(bv->triangles + minInd))->p1->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p2->x, (*(bv->triangles + minInd))->p2->y, (*(bv->triangles + minInd))->p2->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p3->x, (*(bv->triangles + minInd))->p3->y, (*(bv->triangles + minInd))->p3->z);
				printf("max (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p1->x, (*(bv->triangles + maxInd))->p1->y, (*(bv->triangles + maxInd))->p1->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p2->x, (*(bv->triangles + maxInd))->p2->y, (*(bv->triangles + maxInd))->p2->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p3->x, (*(bv->triangles + maxInd))->p3->y, (*(bv->triangles + maxInd))->p3->z);
				*(low + i) = *(bv->triangles + minInd);
				*(high + i) = *(bv->triangles + maxInd);
				// swap to get rid of those elements in the bucket
//				printf("swap %d into %d \n", 2 * i, minInd);
//				printf("swap %d into %d \n", 2 * i + 1, maxInd);

//				float a = (*(bv->triangles + minInd))->p2->x;
//				float b = (*(bv->triangles + 2 * i))->p2->x;
//				printf("%2.f %.2f \n", a, b);

				*(bv->triangles + minInd) = *(bv->triangles + 2 * i);
				*(bv->triangles + maxInd) = *(bv->triangles + 2 * i + 1);

//				a = (*(bv->triangles + minInd))->p2->x;
//				b = (*(bv->triangles + 2 * i))->p2->x;
//				printf("%2.f %.2f \n", a, b);
			}
			for (int i = 0; i < nLow; i++) {
				printf("lows (%.2f %.2f %.2f) \n", low[i]->p1->x, low[i]->p1->y, low[i]->p1->z);
				printf("    (%.2f %.2f %.2f) \n", low[i]->p2->x, low[i]->p2->y, low[i]->p2->z);
				printf("    (%.2f %.2f %.2f) \n", low[i]->p3->x, low[i]->p3->y, low[i]->p3->z);
			}
		} else { // split along z
			// only sort the first half because we simply need to split it into two
			for (int i = 0; i < bv->nTriangles / 2; i++) {
				float min = INFINITY;
				float max = -INFINITY;
				int minInd = 0, maxInd = 0;
				for (int j = 2 * i; j < bv->nTriangles; j++) {
					// centroid c in axis (if you divide by 3)
					float c = (*(bv->triangles + j))->p1->z + (*(bv->triangles + j))->p2->z +
										(*(bv->triangles + j))->p3->z;
					printf("%d %d %.2f \n", i, j, c);
					if (c > max) {
						max = c;
						maxInd = j;
					}
					if (c < min) {
						min = c;
						minInd = j;
					}
				}
				printf("low (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p1->x, (*(bv->triangles + minInd))->p1->y, (*(bv->triangles + minInd))->p1->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p2->x, (*(bv->triangles + minInd))->p2->y, (*(bv->triangles + minInd))->p2->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + minInd))->p3->x, (*(bv->triangles + minInd))->p3->y, (*(bv->triangles + minInd))->p3->z);
				printf("max (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p1->x, (*(bv->triangles + maxInd))->p1->y, (*(bv->triangles + maxInd))->p1->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p2->x, (*(bv->triangles + maxInd))->p2->y, (*(bv->triangles + maxInd))->p2->z);
				printf("    (%.2f %.2f %.2f) \n", (*(bv->triangles + maxInd))->p3->x, (*(bv->triangles + maxInd))->p3->y, (*(bv->triangles + maxInd))->p3->z);
				*(low + i) = *(bv->triangles + minInd);
				*(high + i) = *(bv->triangles + maxInd);
				// swap to get rid of those elements in the bucket
//				printf("swap %d into %d \n", 2 * i, minInd);
//				printf("swap %d into %d \n", 2 * i + 1, maxInd);

//				float a = (*(bv->triangles + minInd))->p2->x;
//				float b = (*(bv->triangles + 2 * i))->p2->x;
//				printf("%2.f %.2f \n", a, b);

				*(bv->triangles + minInd) = *(bv->triangles + 2 * i);
				*(bv->triangles + maxInd) = *(bv->triangles + 2 * i + 1);

//				a = (*(bv->triangles + minInd))->p2->x;
//				b = (*(bv->triangles + 2 * i))->p2->x;
//				printf("%2.f %.2f \n", a, b);
			}
			for (int i = 0; i < nLow; i++) {
				printf("lows (%.2f %.2f %.2f) \n", low[i]->p1->x, low[i]->p1->y, low[i]->p1->z);
				printf("    (%.2f %.2f %.2f) \n", low[i]->p2->x, low[i]->p2->y, low[i]->p2->z);
				printf("    (%.2f %.2f %.2f) \n", low[i]->p3->x, low[i]->p3->y, low[i]->p3->z);
			}
		}

		BoundingVolume *children =
				(BoundingVolume *)malloc(sizeof(BoundingVolume) * 2);

		children[0].triangles = low;
		children[0].nTriangles = nLow;
		children[1].triangles = high;
		children[1].nTriangles = nHigh;

		bv->children = children;
		bv->nChildren = 2;
		bv->nTriangles = 0;

		constructBoundingVolumes(children + 0);
		constructBoundingVolumes(children + 1);
	}

	return bv;
}

// find intersection recursive
// in: scene, ray, node
// if (node is leaf)
//   intersect both and check
// if (ray.intersects node) {
//   find intersection (node.child 1)
//   find intersection (node.child 2)
// }
Intersection3D *findIntersectionBV(BoundingVolume *scene, Ray3D *ray) {
}

// Gets called 236k times
// Copies 24 bytes
// = 5.6 megabytes for HD
// deep copy an intersection object
Intersection3D *copyIntersection(Intersection3D *o, Intersection3D *i) {
	o->point->x = i->point->x;
	o->point->y = i->point->y;
	o->point->z = i->point->z;
	o->exists = i->exists;

	return o;
}

// finds an intersection between the ray and the scene
// returns 0 if no intersection found.
// TODO: prevent dynamic allocation of intersection objects
Intersection3D *findIntersection(Scene *scene, Ray3D *ray) {

	Intersection3D *intersection =
			(Intersection3D *)malloc(sizeof(Intersection3D));
	intersection->point = (Vector3D *)malloc(sizeof(Vector3D));
	intersection->exists = 0;

	Intersection3D *tempIntersection =
			(Intersection3D *)malloc(sizeof(Intersection3D));
	tempIntersection->point = (Vector3D *)malloc(sizeof(Vector3D));

	float minDist = FLT_MAX;
	float tempDist;

	for (int i = 0; i < scene->nTriangles; i++) {
		tempIntersection->exists = 1;
		intersect(ray, scene->triangles + i, tempIntersection);

		if (tempIntersection->exists) {
			tempDist = dist(tempIntersection->point, ray->p);
			if (tempDist < minDist) {
				copyIntersection(intersection, tempIntersection);
				intersection->triangle = scene->triangles + i;
				minDist = tempDist;
			}
		}
	}

	free(tempIntersection);

	intersection->originalRay = ray;

	return intersection->exists ? intersection : 0;
}

// gets a color from a specific intersection and calls rayTrace recursively
// handles all light in the program
float *getColor(Camera *camera, Scene *scene, Intersection3D *intersection,
								int depth, float *rgb) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

	//	printf("gc1 \n");

	Triangle3D *t = intersection->triangle;

	Vector3D normal = *t->plane->v; // normal
	if (dot(intersection->originalRay->v, &normal) > 0) {
		sub(&ORIGIN_3D, &normal, &normal);
	}
	Vector3D *view = intersection->originalRay->v;
	//	printf("gc2 \n");

	float ia = scene->ambientLight;
	float ie = t->k_e;

	float id = 0;
	float is = 0;

	calcPointLights(scene, intersection, &normal, view, &id, &is);
	calcDirectionalLights(scene, intersection, &normal, view, &id, &is);
	calcSpotLights(scene, intersection, &normal, view, &id, &is);

	// TODO: refraction/transparency

	// reflection
	float ir_r = 0;
	float ir_g = 0;
	float ir_b = 0;
	if (depth > 0) {
		Vector3D reflectedVector;
		sub(view, mul(&normal, 2 * dot(view, &normal), &reflectedVector),
				&reflectedVector);
		Ray3D reflectedRay;
		Vector3D rrp, rrv;
		reflectedRay.p = &rrp;
		reflectedRay.v = &rrv;
		Vector3D eps;
		add(intersection->point, mul(&normal, .0001f, &eps), reflectedRay.p);
		reflectedRay.v = &reflectedVector;

		float reflectedColors[3];
		traceRay(camera, scene, &reflectedRay, depth - 1, reflectedColors);

		ir_r = t->k_s * reflectedColors[0];
		ir_g = t->k_s * reflectedColors[1];
		ir_b = t->k_s * reflectedColors[2];
	}

	rgb[0] = t->colorR * (ia + id + ie) + is + ir_r;
	rgb[1] = t->colorG * (ia + id + ie) + is + ir_g;
	rgb[2] = t->colorB * (ia + id + ie) + is + ir_b;

	return rgb;
}

void calcPointLights(Scene *scene, Intersection3D *intersection,
										 Vector3D *normal, Vector3D *view, float *id, float *is) {

	for (int i = 0; i < scene->nPointLights; i++) {

		PointLight *pl;
		pl = &(scene->pointLights[i]);

		float d = dist(intersection->point, pl->point);

		// shadows
		Ray3D lightToInter;
		Vector3D l;
		sub(intersection->point, pl->point, &l);
		norm(&l, &l);

		float lightNormalDot = dot(&l, normal);
		// check if light behind triangle
		if (lightNormalDot > 0) { // light points into triangle
			continue;
		}

		lightToInter.v = &l;
		lightToInter.p = pl->point;

		// shadow
		Intersection3D *inter = findIntersection(scene, &lightToInter);
		if (inter != 0 && dist(inter->point, pl->point) < d - .001f) {
			continue; // not count this light source
		}

		Vector3D lightReflectedVector;
		sub(&l, mul(normal, 2 * lightNormalDot, &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D dvec = {1, d, d * d};
		float il = pl->intensity; // intensity of light
		il /= dot(pl->attenuationCoeffs, &dvec);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, &l) * il;

		// specular reflection
		float is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s *
						 powf(is_temp, scene->kSpecularExponent) * il;
		}
	}
}

void calcDirectionalLights(Scene *scene, Intersection3D *intersection,
													 Vector3D *normal, Vector3D *view, float *id,
													 float *is) {

	for (int i = 0; i < scene->nDirectionalLights; i++) {

		DirectionalLight *dl = &scene->directionalLights[i];

		// check if light behind triangle
		if (dot(dl->direction, normal) > 0) {
			continue;
		}

		// shadows
		Ray3D lightToInter;
		Vector3D ltiv;
		Vector3D ltip;
		lightToInter.v = &ltiv;
		lightToInter.p = &ltip;
		mul(dl->direction, -1, lightToInter.v);
		add(intersection->point, mul(normal, 1.001f, lightToInter.p),
				lightToInter.p);

		Intersection3D *inter = findIntersection(scene, &lightToInter);
		if (inter != 0) {
			continue; // not count this light source
		}

		Vector3D lightReflectedVector;
		sub(dl->direction,
				mul(normal, 2 * dot(dl->direction, normal), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		float il = dl->intensity; // intensity of light

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, dl->direction) * il;

		// specular reflection
		float is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s *
						 powf(is_temp, scene->kSpecularExponent) * il;
		}
	}
}

void calcSpotLights(Scene *scene, Intersection3D *intersection,
										Vector3D *normal, Vector3D *view, float *id, float *is) {

	for (int i = 0; i < scene->nSpotLights; i++) {

		SpotLight *sl = &scene->spotLights[i];
		float d = dist(sl->point, intersection->point);

		Vector3D lightReflectedVector;
		sub(sl->direction,
				mul(normal, 2 * dot(sl->direction, normal), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D lightToInter;
		sub(intersection->point, sl->point, &lightToInter);

		// check if light behind triangle
		if (dot(&lightToInter, normal) > 0 || dot(sl->direction, normal) > 0) {
			continue;
		}

		// shadows
		Ray3D lightToInterRay;
		lightToInterRay.v = &lightToInter;
		lightToInterRay.p = sl->point;
		Intersection3D *inter = findIntersection(scene, &lightToInterRay);
		if (inter != 0 && dist(inter->point, sl->point) < d - .001f) {
			continue; // not count this light source
		}

		Vector3D dvec = {1, d, d * d};
		float il = sl->intensity; // intensity of light
		il /= dot(sl->attenuationCoeffs, &dvec);
		il *= dot(sl->direction, &lightToInter);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, sl->direction) * il;

		// specular reflection
		float is_temp = -dot(view, &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s *
						 powf(is_temp, scene->kSpecularExponent) * il;
		}
	}
}
