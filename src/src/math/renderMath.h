#pragma once

typedef struct {
	float x, y, z;
} Vector3D;

typedef struct {
	float x, y;
} Vector2D;

typedef struct {
	Vector3D v;
	float d;
} Plane3D;

typedef struct {
	Vector3D *points;
	int npoints;
} Polygon3D;

typedef struct {
	Vector2D *points;
	int npoints;
} Polygon2D;

typedef struct {
	Vector3D p1, p2, p3;
	Plane3D plane;
} Triangle3D;

typedef struct {
	Vector2D p1, p2, p3;
} Triangle2D;

typedef struct {
	Vector3D p;
	Vector3D v;
} Line3D;

typedef struct {
	Vector3D p1, p2;
} LineSegment3D;

typedef struct {
	Vector3D p;
	Vector3D v;
} Ray3D;

typedef struct {
	Vector3D point;
	Triangle3D *triangle;
	Ray3D *originalRay;
	Ray3D *reflectionRay;
	Ray3D *refractionRay;
} Intersection3D;

float dot(Vector3D *a, Vector3D *b);
float dot2D(Vector2D *a, Vector2D *b);
Vector3D *add(Vector3D *a, Vector3D *b);
Vector3D *sub(Vector3D *a, Vector3D *b);
Vector3D *mul(float f, Vector3D *v);
Vector3D *div(Vector3D *v, float f);
Vector3D *cross(Vector3D *a, Vector3D *b);
float mag(Vector3D *v);
float mag2D(Vector2D *v);
float dist(Vector3D *a, Vector3D *b);
Intersection3D *intersect(Ray3D *r, Triangle3D *t);
void calculatePlane(Triangle3D *t);
