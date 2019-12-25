#include "renderer.h"

char *rayCast(Camera *camera, Scene *scene, int width, int height) {}

// TODO change basically everything
Ray3D *constructRayThroughPixel(Camera *camera, int x, int y) {
	Ray3D *ray = (Ray3D *)malloc(sizeof(Ray3D));
	ray->p = camera->pos;
	// ray->v = camera->dir; // for (0, 0)

	// suppose camera is pointing at origin from +z axis
	ray->v.dx = x;
	ray->v.dy = y;
	ray->v.dz = camera->pos.z - camera->screenZ;
	return ray;
}

char *traceRay(Scene *scene, Ray3D *ray);

void findIntersection(Scene *scene, Ray3D *ray);

void getColor(); // TBD based on traceRay implementation
