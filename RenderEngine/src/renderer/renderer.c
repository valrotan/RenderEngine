#include "renderer.h"
#include "float.h"
#include "stdlib.h"

void rendererInit(Renderer *renderer) {
	for (int i = 0; i < renderer->scene->nTriangles; i++) {
		Triangle3D *t = &renderer->scene->triangles[i];
		Vector3D *v1 = sub(t->p1, t->p3);
		Vector3D *v2 = sub(t->p1, t->p2);
		t->plane = (Plane3D *)malloc(sizeof(Plane3D));
		t->plane->v = cross(v1, v2);
		t->plane->v = norm(t->plane->v);
		t->plane->d = dot(v1, t->plane->v);
	}
}

void rayCast(Camera *camera, Scene *scene, unsigned char *screen, int width,
						 int height) {
	Ray3D *ray;
	unsigned char *p = screen;

	int halfWidth = width / 2;
	int halfHeight = height / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			ray = constructRayThroughPixel(camera, x - halfWidth, y - halfHeight);

			// start temp code
			//			if (findIntersection(scene, ray) != 0) {
			//				*p++ = 255;
			//				*p++ = 255;
			//				*p++ = 255;
			//			} else {
			//				*p++ = 0;
			//				*p++ = 0;
			//				*p++ = 0;
			//			}
			//			free(ray);
			// end temp code
			unsigned char temp = *traceRay(scene, ray);
			*p++ = temp;
			*p++ = temp;
			*p++ = temp;
			//   find intersection
			//   get color + trace ray (for each intersection bounced ray)
		}
	}
}

// TODO change basically everything
Ray3D *constructRayThroughPixel(Camera *camera, int x, int y) {
	Ray3D *ray = (Ray3D *)malloc(sizeof(Ray3D));
	ray->p = &camera->pos;
	// ray->v = camera->dir; // for (0, 0)

	// suppose camera is pointing at origin from +z axis
	ray->v = (Vector3D *)malloc(sizeof(Vector3D));
	ray->v->x = x;
	ray->v->y = y;
	ray->v->z = -camera->screenZ;
	return ray;
}

Intersection3D *findIntersection(Scene *scene, Ray3D *ray) {

	Intersection3D *intersection = 0;
	Intersection3D *tempIntersection;
	float minDist = FLT_MAX;
	float tempDist;

	for (int i = 0; i < scene->nTriangles; i++) {
		tempIntersection = intersect(ray, &scene->triangles[i]);
		//		Vector3D *r = tempIntersection->point;
		//		printf("%f %f %f\n", r->x, r->y, r->z);
		if (tempIntersection->point != 0) {
			//			printf("%f\n", dist(&ray->p,
			// tempIntersection->point));
			if ((tempDist = dist(ray->p, tempIntersection->point)) < minDist) {
				free(intersection);
				intersection = tempIntersection;
				minDist = tempDist;
			}
		}
	}
	//	if (intersection != 0) {
	//		Vector3D *r = intersection->point;
	//		printf("found intersection (%f %f %f)\n", r->x, r->y, r->z);
	//	}
	return intersection;
}

unsigned char *traceRay(Scene *scene, Ray3D *ray) {
	//	 findIntersection()
	// Intersection {
	//   // could be a pointer to the original I guess
	//   // could also be a pointer to a surface wrapper of triangle
	//   // to store information like reflectivity
	//   triangle t
	//   point p
	//   vector *bouncedVectors
	//   float *bouncedCoefs
	//   int nBouncedVectors
	// };
	// light = getColor(scene, intersection)
	// for bounced vectors in intersection
	//   light += bounced coef * traceRay(scene, bounced ray)

	Intersection3D *intersection = findIntersection(scene, ray);
	unsigned char *color;
	if (intersection != 0) {
		color = getColor(scene, intersection);
	} else {
		color = (unsigned char *)malloc(sizeof(unsigned char));
		*color = 0;
	}
	free(intersection);
	return color;
	//	return 0;
}

unsigned char *getColor(Scene *scene, Intersection3D *intersection) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

	float i = 0;

	Vector3D *l = sub(scene->pointLights->point, intersection->point); // hmmmm...
	l = norm(l);
	Vector3D *n = intersection->triangle->plane->v;
	float K_D = .5;
	float id = K_D * dot(n, l) * scene->pointLights->intensity;

	i = id;

	unsigned char *luminosity = (unsigned char *)malloc(sizeof(char) * 1);
	printf("%f\n", i);
	if (i > 1) {
		*luminosity = 255;
	} else if (i < 0) {
		*luminosity = 0;
	} else {
		*luminosity = (unsigned char)(id * 255);
	}
	return luminosity;
}
