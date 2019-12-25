#include "rendererTests.h"

// TODO: come up with tests
void runRendererTests() {

	Camera camera;
	Point3D pos = {0, 0, 150};
	Vector3D dir = {0, 0, -1};
	Vector3D up = {0, 1, 0};
	camera.pos = pos;
	camera.dir = dir;
	camera.up = up;
	camera.screenZ = 150;

	Ray3D *ray = constructRayThroughPixel(&camera, 0, 0);
	printf("Ray (%f, %f, %f) towards <%f, %f, %f>\n", ray->p.x, ray->p.y,
				 ray->p.z, ray->v.dx, ray->v.dy, ray->v.dz);

	ray = constructRayThroughPixel(&camera, 1, 0);
	printf("Ray (%f, %f, %f) towards <%f, %f, %f>\n", ray->p.x, ray->p.y,
				 ray->p.z, ray->v.dx, ray->v.dy, ray->v.dz);

	ray = constructRayThroughPixel(&camera, 0, 1);
	printf("Ray (%f, %f, %f) towards <%f, %f, %f>\n", ray->p.x, ray->p.y,
				 ray->p.z, ray->v.dx, ray->v.dy, ray->v.dz);
	// TODO: add actual tests
}
