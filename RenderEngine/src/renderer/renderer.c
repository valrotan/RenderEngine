#include "renderer.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

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
		printf("  color : C (%d, %d, %d) \n", t->colorR, t->colorG, t->colorB);
		printf("  refl  : C (%.2f, %.2f, %.2f) \n", t->k_d, t->k_e, t->k_s);
	}
}

typedef struct {
	Renderer *renderer;
	unsigned char *screen;
	int width;
	int height;
	int startLine;
	int stopLine;
} RendererSegment;

void *rayTraceSegment(void *pSegment) {

	RendererSegment *rSegment = (RendererSegment *)pSegment;

	Ray3D *ray;
	unsigned char *p = rSegment->screen;

	for (int y = rSegment->startLine; y < rSegment->stopLine; y++) {
		for (int x = 0; x < rSegment->width; x++) {
			ray = constructRayThroughPixel(rSegment->renderer->camera, x, y,
																		 rSegment->width, rSegment->height);

			unsigned char *i = traceRay(rSegment->renderer->camera,
																	rSegment->renderer->scene, ray, 3);
			*p++ = i[0];
			*p++ = i[1];
			*p++ = i[2];
		}
	}
	return NULL;
}

void rayTrace(Renderer *renderer, unsigned char *screen, int width,
							int height) {

	pthread_t *threads =
			(pthread_t *)malloc(sizeof(pthread_t) * renderer->nThreads);
	RendererSegment *rSegments =
			(RendererSegment *)malloc(sizeof(RendererSegment) * renderer->nThreads);
	double lines = (double)height / renderer->nThreads;

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
	double aspectRatio = (double)imageWidth / imageHeight;
	double scale = tanf(getRad(camera->fov / 2));

	// TODO: move all the constants to the loop in rayTrace
	double Px = (2 * (x + 0.5f) / imageWidth - 1) * scale * aspectRatio;
	double Py = (1 - 2 * (y + 0.5f) / imageHeight) * scale;

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
unsigned char *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int depth) {

	Intersection3D *intersection = findIntersection(scene, ray);
	unsigned char *color;
	//	printf("tr1 \n");
	if (intersection != 0) {
		color = getColor(camera, scene, intersection, depth);
		free(intersection);
	} else {
		color = (unsigned char *)malloc(sizeof(char) * 3);
		color[0] = scene->bkgR; // background light
		color[1] = scene->bkgG; // background light
		color[2] = scene->bkgB; // background light
	}
	return color;
	//	return 0;
}

// Gets called 236k times
// Copies 24 bytes
// = 5.6 megabytes for HD
// deep copy an intersection object
Intersection3D *copyIntersection(Intersection3D *o, Intersection3D *i) {
	o->point->x = i->point->x;
	o->point->y = i->point->y;
	o->point->z = i->point->z;
	o->triangle = i->triangle;
	o->originalRay = i->originalRay;
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

	double minDist = FLT_MAX;
	double tempDist;

	for (int i = 0; i < scene->nTriangles; i++) {
		tempIntersection->exists = 1;
		intersect(ray, &scene->triangles[i], tempIntersection);

		if (tempIntersection->exists) {
			tempDist = dist(tempIntersection->point, ray->p);
			if (tempDist < minDist) {
				copyIntersection(intersection, tempIntersection);
				minDist = tempDist;
			}
		}
	}

	return intersection->exists ? intersection : 0;
}

// gets a color from a specific intersection and calls rayTrace recursively
// handles all light in the program
unsigned char *getColor(Camera *camera, Scene *scene,
												Intersection3D *intersection, int depth) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

	//	printf("gc1 \n");

	double i[3];

	Triangle3D *t = intersection->triangle;

	Vector3D normal = *t->plane->v; // normal
	if (dot(intersection->originalRay->v, &normal) > 0) {
		sub(&ORIGIN_3D, &normal, &normal);
	}
	Vector3D *view = intersection->originalRay->v;
	//	printf("gc2 \n");

	double ia = scene->ambientLight;
	double ie = t->k_e;

	double id = 0;
	double is = 0;

	calcPointLights(scene, intersection, &normal, view, &id, &is);
	calcDirectionalLights(scene, intersection, &normal, view, &id, &is);
	calcSpotLights(scene, intersection, &normal, view, &id, &is);

	// TODO: refraction

	// reflection
	double ir_r = 0;
	double ir_g = 0;
	double ir_b = 0;
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

		unsigned char *reflectedColors =
				traceRay(camera, scene, &reflectedRay, depth - 1);

		ir_r = t->k_s * reflectedColors[0] / 255;
		ir_g = t->k_s * reflectedColors[1] / 255;
		ir_b = t->k_s * reflectedColors[2] / 255;
	}

	i[0] = t->colorR * (ia + id + ie) / 255.0f + is + ir_r;
	i[1] = t->colorG * (ia + id + ie) / 255.0f + is + ir_g;
	i[2] = t->colorB * (ia + id + ie) / 255.0f + is + ir_b;

	unsigned char *i_char = (unsigned char *)malloc(sizeof(char) * 3);
	if (i[0] > 1) {
		i_char[0] = 255;
	} else if (i[0] < 0) {
		i_char[0] = 0;
	} else {
		i_char[0] = (unsigned char)(i[0] * 255);
	}
	if (i[1] > 1) {
		i_char[1] = 255;
	} else if (i[1] < 0) {
		i_char[1] = 0;
	} else {
		i_char[1] = (unsigned char)(i[1] * 255);
	}
	if (i[2] > 1) {
		i_char[2] = 255;
	} else if (i[2] < 0) {
		i_char[2] = 0;
	} else {
		i_char[2] = (unsigned char)(i[2] * 255);
	}

	return i_char;
}

void calcPointLights(Scene *scene, Intersection3D *intersection,
										 Vector3D *normal, Vector3D *view, double*id, double*is) {

	//	printf("%p %p \n", pl, scene->pointLights);
	//	printf("(%.2f, %.2f, %.2f)", scene->pointLights->point->x,
	// scene->pointLights->point->y, scene->pointLights->point->z);
	for (int i = 0; i < scene->nPointLights; i++) {

		PointLight *pl;
		pl = &(scene->pointLights[i]);

		double d = dist(intersection->point, pl->point);

		// shadows
		Ray3D lightToInter;
		Vector3D l;
		sub(intersection->point, pl->point, &l);
		norm(&l, &l);

		lightToInter.v = &l;
		lightToInter.p = pl->point;

		Intersection3D *inter = findIntersection(scene, &lightToInter);
		if (inter != 0 && dist(inter->point, pl->point) < d - .001f) {
			continue; // not count this light source
		}

		// check if light behind triangle
		if (dot(&l, normal) > 0) {
			continue;
		}

		Vector3D lightReflectedVector;
		sub(&l, mul(normal, 2 * dot(&l, normal), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D dvec = {1, d, d * d};
		double il = pl->intensity; // intensity of light
		il /= dot(pl->attenuationCoeffs, &dvec);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, &l) * il;

		// specular reflection
		double N = 3; // specular light exponent
		double is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s * powf(is_temp, N) * il;
		}
	}
}

void calcDirectionalLights(Scene *scene, Intersection3D *intersection,
													 Vector3D *normal, Vector3D *view, double*id,
	double*is) {

	for (int i = 0; i < scene->nDirectionalLights; i++) {

		DirectionalLight *dl = &scene->directionalLights[i];

		// check if light behind triangle
		if (dot(dl->direction, normal) > 0) {
			continue;
		}

		// shadows
		Ray3D lightToInter;
		lightToInter.v = (Vector3D *)malloc(sizeof(Vector3D));
		lightToInter.p = (Vector3D *)malloc(sizeof(Vector3D));
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

		double il = dl->intensity; // intensity of light

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, dl->direction) * il;

		// specular reflection
		double N = 3; // specular light exponent
		double is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s * powf(is_temp, N) * il;
		}
	}
}

void calcSpotLights(Scene *scene, Intersection3D *intersection,
										Vector3D *normal, Vector3D *view, double*id, double*is) {

	for (int i = 0; i < scene->nSpotLights; i++) {

		SpotLight *sl = &scene->spotLights[i];
		double d = dist(sl->point, intersection->point);

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
		double il = sl->intensity; // intensity of light
		il /= dot(sl->attenuationCoeffs, &dvec);
		il *= dot(sl->direction, &lightToInter);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, sl->direction) * il;

		// specular reflection
		double N = 3; // specular light exponent
		double is_temp = -dot(view, &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s * powf(is_temp, N) * il;
		}
	}
}

// TODO change the structure so the light helper functions only calculate the
// illuminosity
