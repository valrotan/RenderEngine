#include "renderer.h"
#include "stdlib.h"
#include "float.h"

void rayCast(Camera *camera, Scene *scene, char *screen, int width,
						 int height) {
	Ray3D *ray;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			ray = constructRayThroughPixel(camera, x, y);
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
	ray->v.z = camera->pos.z - camera->screenZ;
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

	Intersection3D *intersection =
			(Intersection3D *)malloc(sizeof(Intersection3D));
	Intersection3D *tempIntersection;
	float minDist = FLT_MAX;

	for (int i = 0; i < scene->nTriangles; i++) {
		tempIntersection = intersect(ray, &scene->triangles[i]);
		if (tempIntersection->point != 0) {
			if (1) {

				}
		}
	}
	return intersection;
}

void getColor(void); // TBD based on traceRay implementation
