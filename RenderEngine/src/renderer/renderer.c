#include "renderer.h"
#include "float.h"
#include "stdlib.h"

void rayCast(Camera *camera, Scene *scene, unsigned char *screen, int width,
						 int height) {
	Ray3D *ray;
	unsigned char *p = screen;

	int halfWidth = width / 2;
	int halfHeight = height / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			ray = constructRayThroughPixel(camera, x - halfWidth, y - halfHeight);
			//			printf("%f %f %f\n", ray->v.x, ray->v.y,
			// ray->v.z);
			// start temp code
			if (findIntersection(scene, ray) != 0) {
				*p++ = 255;
				*p++ = 255;
				*p++ = 255;
			} else {
				*p++ = 0;
				*p++ = 0;
				*p++ = 0;
			}
			free(ray);
			// end temp code

			// trace ray (ray)
			//   find intersection
			//   get color + trace ray (for each intersection bounced ray)
		}
	}
}

// TODO change basically everything
Ray3D *constructRayThroughPixel(Camera *camera, int x, int y) {
	Ray3D *ray = (Ray3D *)malloc(sizeof(Ray3D));
	ray->p = camera->pos;
	// ray->v = camera->dir; // for (0, 0)

	// suppose camera is pointing at origin from +z axis
	ray->v.x = x;
	ray->v.y = y;
	ray->v.z = -camera->screenZ;
	return ray;
}

char *traceRay(Scene *scene, Ray3D *ray) {
	// findIntersection()
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
	return 0;
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
			printf("%f\n", dist(&ray->p, tempIntersection->point));
			if ((tempDist = dist(&ray->p, tempIntersection->point)) < minDist) {
				free(intersection);
				intersection = tempIntersection;
				minDist = tempDist;
			}
		}
	}
	if (intersection != 0) {
		Vector3D *r = intersection->point;
		printf("found intersection (%f %f %f)\n", r->x, r->y, r->z);
	}
	return intersection;
}

void getColor(void); // TBD based on traceRay implementation
