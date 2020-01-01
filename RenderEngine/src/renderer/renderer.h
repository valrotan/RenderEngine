#pragma once

#include "../math/renderMath.h"

// todo: change screenZ to fov
typedef struct {
	Vector3D pos;
	Vector3D dir;
	Vector3D up;
	//	float xfov;
	//	float yfov;

	// distance of screen from camera point
	// makes ray through pixel calculation easier
	// for 256x256 150 is a good value
	int screenZ;
} Camera;

// sample camera:
// (150,0,0)
// (-150,0,0)
// (0,0,1)
// 150

// Only triangles for now I guess
typedef struct {
	Triangle3D *triangles;
	int nTriangles;
} Scene;

void rayCast(Camera *camera, Scene *scene, unsigned char *screen, int width,
						 int height);

Ray3D *constructRayThroughPixel(Camera *camera, int x, int y);

char *traceRay(Scene *scene, Ray3D *ray);

Intersection3D *findIntersection(Scene *scene, Ray3D *ray);

void getColor(void); // TBD based on traceRay implementation
