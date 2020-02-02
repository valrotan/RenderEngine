#pragma once

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
	unsigned char colorR, colorG, colorB;
	double k_e; // emission
	double k_d; // diffuse
	double k_s; // specular (reflectivity)
} Triangle3D;

typedef struct {
	Vector3D *p;
	Vector3D *v;
} Ray3D;

typedef struct {
	Vector3D *point;
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
Vector3D applyTransformation(Vector3D originVec, Matrix4x4 transform);

static Vector3D ORIGIN_3D = {0, 0, 0};

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
Intersection3D *intersect(Ray3D *r, Triangle3D *t, Intersection3D *out);
