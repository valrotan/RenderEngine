#pragma once

#include "../math/renderMath.h"

typedef struct {
	Point3D pos;
	Vector3D dir;
	Vector3D up;
	float xfov;
	float yfov;
} Camera;

typedef struct {
	Triangle3D *triangles;
	int nTriangles;
} Scene;

char *rayCast(Camera *camera, Scene *scene, int width, int height);

Ray3D constructRayThroughPixel(Camera *camera, int x, int y);

void traceRay(Scene *scene, Ray3D *ray, char **currentPixel);

void findIntersection(Scene *scene, Ray3D *ray);

void getColor(); // TBD based on traceRay implementation
