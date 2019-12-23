#pragma once

typedef struct {
	float x, y, z;
} Point;

typedef struct {
	float dx, dy, dz;
} Vector;

typedef struct {
	Vector v;
	float d;
} Plane;

typedef struct {
	Point *points;
	int npoints;
} Polygon;

typedef struct {
	Point p1, p2, p3;
} Triangle;

typedef struct {
	Point p;
	Vector v;
} Line;

typedef struct {
	Point p1, p2;
} LineSegment;

typedef struct {
	Point p;
	Vector v;
} Ray;

float dot(Vector *a, Vector *b);
float mag(Vector *v);
