#pragma once

typedef struct {
	float x, y, z;
} Point3D;

typedef struct {
	float x, y;
} Point2D;

typedef struct {
	float dx, dy, dz;
} Vector3D;

typedef struct {
	float dx, dy;
} Vector2D;

typedef struct {
	Vector3D v;
	float d;
} Plane;

typedef struct {
	Point3D *points;
	int npoints;
} Polygon3D;

typedef struct {
	Point2D *points;
	int npoints;
} Polygon2D;

typedef struct {
	Point3D p1, p2, p3;
} Triangle3D;

typedef struct {
	Point2D p1, p2, p3;
} Triangle2D;

typedef struct {
	Point3D p;
	Vector3D v;
} Line3D;

typedef struct {
	Point3D p1, p2;
} LineSegment3D;

typedef struct {
	Point3D p;
	Vector3D v;
} Ray3D;

float dot(Vector3D *a, Vector3D *b);
float dot2D(Vector2D *a, Vector2D *b);
float mag(Vector3D *v);
float mag2D(Vector2D *v);
