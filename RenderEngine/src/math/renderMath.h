#pragma once

#include <stdlib.h>

typedef struct {
	double x, y, z;
} Vector3D;

typedef struct {
	double x, y;
} Vector2D;

typedef struct {
	Vector3D *v;
	double d;
} Plane3D;

typedef struct {
	Vector3D *points;
	int npoints;
} Polygon3D;

typedef struct {
	Vector3D *p1, *p2, *p3;
	Plane3D *plane;
	double colorR, colorG, colorB;
	double k_e; // emission
	double k_d; // diffuse
	double k_s; // specular (reflectivity)
	Vector3D *centroid;
} Triangle3D;

typedef struct {
	Vector3D *p;
	Vector3D *v;
} Ray3D;

typedef struct {
	Vector3D point;
	Triangle3D *triangle;
	Ray3D *originalRay;
	int exists;
} Intersection3D;

typedef struct {
	double matrix[4][4];
} Matrix4x4;

double getRad(double);
Matrix4x4 getEye(void);
Matrix4x4 multiply4x4Matrices(Matrix4x4 a, Matrix4x4 b);

Matrix4x4 getTranslationMatrix(double x, double y, double z);
Matrix4x4 getScaleMatrix(double xScale, double yScale, double zScale);
Matrix4x4 getXRotationMatrix(double angle,
														 int rad); // angle is angle in degrees or radians
Matrix4x4 getYRotationMatrix(double angle,
														 int rad); // specify if rad with rad = 1
Matrix4x4 getZRotationMatrix(double angle, int rad); // else rad = 0
Matrix4x4 getTransformationMatrix(Matrix4x4 matrices[], int size);
Vector3D *applyTransformation(Vector3D *orig, Matrix4x4 *transform,
															Vector3D *out);

static Vector3D ORIGIN_3D = {0, 0, 0};
static Vector3D AXIS_X = {1, 0, 0};
static Vector3D AXIS_Y = {0, 1, 0};
static Vector3D AXIS_Z = {0, 0, 1};
static double PI = 3.14159265359;
static double E = 2.7182818284590452353602874713527;

double dot(Vector3D *a, Vector3D *b);
double dot2D(Vector2D *a, Vector2D *b);
Vector3D *add(Vector3D *a, Vector3D *b, Vector3D *out);
Vector3D *sub(Vector3D *a, Vector3D *b, Vector3D *out);
Vector3D *mul(Vector3D *v, double f, Vector3D *out);
Vector3D *divide(Vector3D *v, double f, Vector3D *out);
Vector3D *cross(Vector3D *a, Vector3D *b, Vector3D *out);
Vector3D *norm(Vector3D *v, Vector3D *out);
double mag(Vector3D *v);
double mag2D(Vector2D *v);
double dist(Vector3D *a, Vector3D *b);
double distSqrd(Vector3D *a, Vector3D *b);
Intersection3D *intersect(Ray3D *r, Triangle3D *t, Intersection3D *out);
void setMinMax(const double *point, double *min, double *max);

Vector3D *triangleGetX(Triangle3D *t, Vector3D *v);
Vector3D *triangleGetY(Triangle3D *t, Vector3D *v);
Vector3D *triangleGetZ(Triangle3D *t, Vector3D *v);

double triangleCentroidX(Triangle3D *);
double triangleCentroidY(Triangle3D *);
double triangleCentroidZ(Triangle3D *);

double dotOne(Vector3D *v);
