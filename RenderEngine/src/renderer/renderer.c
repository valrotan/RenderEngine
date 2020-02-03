#include "renderer.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

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
	printf("%% children: %d triangles: %d \n", bv->nChildren, bv->nTriangles);
	for (int i = 0; i < bv->nTriangles; i++) {
		printf("(%.2f %.2f %.2f) ", bv->triangles[i]->colorR,
					 bv->triangles[i]->colorG, bv->triangles[i]->colorB);
	}
	printf("\n");
	printf("drawBox(%c, %c, 1) \n", c - 2, c - 1);
	//	}
	//	printf("-----\n");
}

void rendererInit(Renderer *renderer) {

	Camera *cam = renderer->camera;
	cam->scale = tanf(getRad(cam->fov / 2));
	cam->aspectRatio = (float)(cam->width) / cam->height;

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

//		printf("init triangle : N (%.2f, %.2f, %.2f) d %.2f \n", t->plane->v->x,
//					 t->plane->v->y, t->plane->v->z, t->plane->d);
//		printf("  color : C (%f, %f, %f) \n", t->colorR, t->colorG, t->colorB);
//		printf("  refl  : C (%.2f, %.2f, %.2f) \n", t->k_d, t->k_e, t->k_s);
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

	renderer->scene->bv = constructBoundingVolumes(bv);
	//	printf("finished construction \n");

	//	printf("%d \n", bv->nChildren);
	//	printf("%d \n", bv->children->nChildren);
	//	printBoundingVolume(bv);
	//	printf("low  (%.2f, %.2f, %.2f) \n", bv->low.x, bv->low.y, bv->low.z);
	//	printf("high (%.2f, %.2f, %.2f) \n", bv->high.x, bv->high.y, bv->high.z);
}

void *rayTraceSegment(void *pSegment) {

	RendererSegment *rSegment = (RendererSegment *)pSegment;

	Ray3D ray;
	Vector3D rp, rv;
	ray.p = &rp;
	ray.v = &rv;
	applyTransformation(&ORIGIN_3D, &rSegment->renderer->camera->cameraToWorld, ray.p);

	unsigned char *p = rSegment->screen;

	float rgb[3];
	for (int y = rSegment->startLine; y < rSegment->stopLine; y++) {
		for (int x = 0; x < rSegment->width; x++) {
			constructRayThroughPixel(rSegment->renderer->camera, x, y,
															 rSegment->width, rSegment->height, &ray);

			traceRay(rSegment->renderer->camera, rSegment->renderer->scene, &ray, 3,
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

// TODO: split in grid maybe to utilize threads more
// Fun thing to try: plot execution time of thread vs line number to see
//     how threads are utilized when split this way
void rayTrace(Renderer *renderer, unsigned char *screen) {

	pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * //
																					 (size_t)renderer->nThreads);
	RendererSegment *rSegments = (RendererSegment *)malloc(
			sizeof(RendererSegment) * (size_t)renderer->nThreads);
	float lines = (float) (renderer->camera->height) / renderer->nThreads;

	for (int i = 0; i < renderer->nThreads; i++, rSegments++) {
		rSegments->renderer = renderer;
		rSegments->screen = screen + 3 * renderer->camera->width * (int)(i * lines + .5f);
		rSegments->width = renderer->camera->width;
		rSegments->height = renderer->camera->height;
		rSegments->startLine = (int)(i * lines + .5f);
		rSegments->stopLine = (int)((i + 1) * lines + .5f);
		pthread_create(threads + i, NULL, rayTraceSegment, (void *)rSegments);
	}
	for (int i = 0; i < renderer->nThreads; i++) {
		pthread_join(threads[i], NULL);
	}
	free(threads);
}

// Constructs a ray that originates at the camera position and shoots through
// the given pixel (x,y)
// MBP performance: ~50ms for HD on single threadb
Ray3D *constructRayThroughPixel(Camera *camera, int x, int y, int imageWidth,
																int imageHeight, Ray3D *ray) {

	// TODO: move all the constants to the loop in rayTrace
	float Px = (2 * (x + 0.5f) / imageWidth - 1) * camera->scale * camera->aspectRatio;
	float Py = (1 - 2 * (y + 0.5f) / imageHeight) * camera->scale;

	Vector3D pWorld = {Px, Py, -1};
	applyTransformation(&pWorld, &camera->cameraToWorld, &pWorld);

	sub(&pWorld, ray->p, ray->v);
	norm(ray->v, ray->v);

	return ray;
}

// recursive ray tracing for depth levels
// TODO: prevent excess dynamic allocation for pixel
float *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int depth,
								float *rgb) {

	Intersection3D intersection;
	intersection.exists = 0;

	findIntersectionBV(scene->bv, ray, &intersection);
	intersection.originalRay = ray;

	if (intersection.exists) {
		getColor(camera, scene, &intersection, depth, rgb);
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

	if (bv->nTriangles == 0) {
		bv->low = ORIGIN_3D;
		bv->high = ORIGIN_3D;
		bv->nChildren = 0;
		return bv;
	}
	// determine own bounding volume points
	{
		Triangle3D *cur = bv->triangles[0];

		bv->low.x = fminf(fminf(cur->p1->x, cur->p2->x), cur->p3->x);
		bv->low.y = fminf(fminf(cur->p1->y, cur->p2->y), cur->p3->y);
		bv->low.z = fminf(fminf(cur->p1->z, cur->p2->z), cur->p3->z);
		bv->high.x = fmaxf(fmaxf(cur->p1->x, cur->p2->x), cur->p3->x);
		bv->high.y = fmaxf(fmaxf(cur->p1->y, cur->p2->y), cur->p3->y);
		bv->high.z = fmaxf(fmaxf(cur->p1->z, cur->p2->z), cur->p3->z);

		for (int i = 1; i < bv->nTriangles; i++) {
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

	// end at n triangles per volume
	if (bv->nTriangles > 1) {
		int nLow = bv->nTriangles / 2;
		int nHigh = bv->nTriangles - nLow;

		Triangle3D **low =
				(Triangle3D **)malloc(sizeof(Triangle3D *) * (size_t)nLow);
		Triangle3D **high = (Triangle3D **)malloc( //
				sizeof(Triangle3D *) * (size_t)nHigh);

		Vector3D bvDims;
		sub(&bv->high, &bv->low, &bvDims);
		//		printf("dims (%.2f, %.2f, %.2f) \n", bvDims.x, bvDims.y, bvDims.z);

		if (bvDims.x >= bvDims.y && bvDims.x >= bvDims.z) { // split along x
			// sort using x
			for (int i = 0; i < bv->nTriangles - 1; i++) {
				float min = INFINITY;
				int minInd = i;
				for (int j = i; j < bv->nTriangles; j++) {
					// centroid c in axis (if you divide by 3)
					float c = bv->triangles[j]->p1->x + //
										bv->triangles[j]->p2->x + //
										bv->triangles[j]->p3->x;
					if (c < min) {
						min = c;
						minInd = j;
					}
				}
				Triangle3D *temp = bv->triangles[minInd];
				bv->triangles[minInd] = bv->triangles[i];
				bv->triangles[i] = temp;
			}
		} else if (bvDims.y >= bvDims.x && bvDims.y >= bvDims.z) { // split along y
			// sort using y
			for (int i = 0; i < bv->nTriangles - 1; i++) {
				float min = INFINITY;
				int minInd = i;
				for (int j = i; j < bv->nTriangles; j++) {
					// centroid c in axis (if you divide by 3)
					float c = bv->triangles[j]->p1->y + //
										bv->triangles[j]->p2->y + //
										bv->triangles[j]->p3->y;
					if (c < min) {
						min = c;
						minInd = j;
					}
				}
				Triangle3D *temp = bv->triangles[minInd];
				bv->triangles[minInd] = bv->triangles[i];
				bv->triangles[i] = temp;
			}
		} else { // split along z
			// sort using z
			for (int i = 0; i < bv->nTriangles - 1; i++) {
				float min = INFINITY;
				int minInd = i;
				for (int j = i; j < bv->nTriangles; j++) {
					// centroid c in axis (if you divide by 3)
					float c = bv->triangles[j]->p1->z + //
										bv->triangles[j]->p2->z + //
										bv->triangles[j]->p3->z;
					if (c < min) {
						min = c;
						minInd = j;
					}
				}
				Triangle3D *temp = bv->triangles[minInd];
				bv->triangles[minInd] = bv->triangles[i];
				bv->triangles[i] = temp;
			}
		}

		memcpy(low, bv->triangles, sizeof(Triangle3D *) * (size_t)nLow);
		memcpy(high, bv->triangles + nLow, sizeof(Triangle3D *) * (size_t)nHigh);
		BoundingVolume *children =
				(BoundingVolume *)malloc(sizeof(BoundingVolume) * 2);

		children[0].triangles = low;
		children[0].nTriangles = nLow;
		children[1].triangles = high;
		children[1].nTriangles = nHigh;

		bv->children = children;
		bv->nChildren = 2;
		bv->nTriangles = 0;

		constructBoundingVolumes(children);
		constructBoundingVolumes(children + 1);
	}

	return bv;
}

// inspiration: http://www.cs.utah.edu/~awilliam/box/box.pdf
int smitsBoxIntersect(BoundingVolume *bv, Ray3D *ray) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	float divx = 1 / ray->v->x;
	if (divx >= 0) {
		tmin = (bv->low.x - ray->p->x) * divx;
		tmax = (bv->high.x - ray->p->x) * divx;
	} else {
		tmin = (bv->high.x - ray->p->x) * divx;
		tmax = (bv->low.x - ray->p->x) * divx;
	}
	float divy = 1 / ray->v->y;
	if (divy >= 0) {
		tymin = (bv->low.y - ray->p->y) * divy;
		tymax = (bv->high.y - ray->p->y) * divy;
	} else {
		tymin = (bv->high.y - ray->p->y) * divy;
		tymax = (bv->low.y - ray->p->y) * divy;
	}
	if ((tmin > tymax) || (tymin > tmax))
		return 0;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	float divz = 1 / ray->v->z;
	if (divz >= 0) {
		tzmin = (bv->low.z - ray->p->z) * divz;
		tzmax = (bv->high.z - ray->p->z) * divz;
	} else {
		tzmin = (bv->high.z - ray->p->z) * divz;
		tzmax = (bv->low.z - ray->p->z) * divz;
	}
	if ((tmin > tzmax) || (tzmin > tmax))
		return 0;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return (tmax > 0);
}

// if intersects bounding volume:
// if bv.nTriangles > 0
//   find min intersection with triangles
// if bv.nChildren > 0
//   find min intersection with bounding volumes
// return min
Intersection3D *findIntersectionBV(BoundingVolume *bv, Ray3D *ray,
																	 Intersection3D *inter) {

	if (smitsBoxIntersect(bv, ray)) {

		Intersection3D tempIntersection;
		tempIntersection.exists = 0;

		float minDist = FLT_MAX;
		float tempDist;

		// triangle intersections
		for (int i = 0; i < bv->nTriangles; i++) {
			intersect(ray, *(bv->triangles + i), &tempIntersection);

			if (tempIntersection.exists) {
				tempDist = distSqrd(&tempIntersection.point, ray->p);
				if (tempDist < minDist) {
					inter->point = tempIntersection.point;
					inter->triangle = *(bv->triangles + i);
					inter->exists = 1;
					minDist = tempDist;
				}
			}
		}

		// children intersections
		for (int i = 0; i < bv->nChildren; i++) {
			findIntersectionBV(bv->children + i, ray, &tempIntersection);
			if (tempIntersection.exists) {
				tempDist = distSqrd(&tempIntersection.point, ray->p);
				if (tempDist < minDist) {
					inter->point = tempIntersection.point;
					inter->triangle = tempIntersection.triangle;
					inter->exists = 1;
					minDist = tempDist;
				}
			}
		}

		return inter;
	}
	inter->exists = 0;
	return inter;
}

// gets a color from a specific intersection and calls rayTrace recursively
// handles all light in the program
float *getColor(Camera *camera, Scene *scene, Intersection3D *intersection,
								int depth, float *rgb) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

	Triangle3D *t = intersection->triangle;

	Vector3D normal = *t->plane->v; // normal
	if (dot(intersection->originalRay->v, &normal) > 0) {
		sub(&ORIGIN_3D, &normal, &normal);
	}
	Vector3D *view = intersection->originalRay->v;

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
		add(&intersection->point, mul(&normal, .0001f, &eps), reflectedRay.p);
		reflectedRay.v = &reflectedVector;

		float reflectedColors[3];
		traceRay(camera, scene, &reflectedRay, depth - 1, reflectedColors);

		ir_r = t->k_s * reflectedColors[0];
		ir_g = t->k_s * reflectedColors[1];
		ir_b = t->k_s * reflectedColors[2];
	}

	rgb[0] = t->colorR * (scene->ambientLight + id + t->k_e) + is + ir_r;
	rgb[1] = t->colorG * (scene->ambientLight + id + t->k_e) + is + ir_g;
	rgb[2] = t->colorB * (scene->ambientLight + id + t->k_e) + is + ir_b;

	return rgb;
}

void calcPointLights(Scene *scene, Intersection3D *intersection,
										 Vector3D *normal, Vector3D *view, float *id, float *is) {

	for (int i = 0; i < scene->nPointLights; i++) {

		PointLight *pl;
		pl = &(scene->pointLights[i]);

		float d = dist(&intersection->point, pl->point);

		// shadows
		Ray3D lightToInter;
		Vector3D l;
		sub(&intersection->point, pl->point, &l);
		norm(&l, &l);

		float lightNormalDot = dot(&l, normal);
		// check if light behind triangle
		if (lightNormalDot > 0) { // light points into triangle
			continue;
		}

		lightToInter.v = &l;
		lightToInter.p = pl->point;

		// shadow
		Intersection3D inter;
		inter.exists = 0;
		findIntersectionBV(scene->bv, &lightToInter, &inter);
		if (inter.exists && dist(&inter.point, pl->point) < d - .001f) {
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
		add(&intersection->point, mul(normal, 1.001f, lightToInter.p),
				lightToInter.p);

		Intersection3D inter;
		inter.exists = 0;
		findIntersectionBV(scene->bv, &lightToInter, &inter);
		if (inter.exists) {
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
		float d = dist(sl->point, &intersection->point);

		Vector3D lightReflectedVector;
		sub(sl->direction,
				mul(normal, 2 * dot(sl->direction, normal), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D lightToInter;
		sub(&intersection->point, sl->point, &lightToInter);

		// check if light behind triangle
		if (dot(&lightToInter, normal) > 0 || dot(sl->direction, normal) > 0) {
			continue;
		}

		// shadows
		Ray3D lightToInterRay;
		lightToInterRay.v = &lightToInter;
		lightToInterRay.p = sl->point;

		Intersection3D inter;
		inter.exists = 0;
		findIntersectionBV(scene->bv, &lightToInterRay, &inter);
		if (inter.exists && dist(&inter.point, sl->point) < d - .001f) {
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
