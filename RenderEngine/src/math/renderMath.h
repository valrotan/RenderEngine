#pragma once

typedef struct {
	float x, y, z;
} Vector3D;

typedef struct {
	float x, y;
} Vector2D;

typedef struct {
	Vector3D *v;
	float d;
} Plane3D;

typedef struct {
	Vector3D *points;
	int npoints;
} Polygon3D;

typedef struct {
	Vector3D *p1, *p2, *p3;
	Plane3D *plane;
	unsigned char colorR, colorG, colorB;
	float k_e; // emission
	float k_d; // diffuse
	float k_s; // specular (reflectivity)
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
	float matrix[4][4];
} Matrix4x4;

float getRad(float);
Matrix4x4 getEye(void);
Matrix4x4 multiply4x4Matrices(Matrix4x4 a, Matrix4x4 b);

Matrix4x4 getTranslationMatrix(float x, float y, float z);
Matrix4x4 getScaleMatrix(float xScale, float yScale, float zScale);
Matrix4x4 getXRotationMatrix(float angle,
														 int rad); // angle is angle in degrees or radians
Matrix4x4 getYRotationMatrix(float angle,
														 int rad); // specify if rad with rad = 1
Matrix4x4 getZRotationMatrix(float angle, int rad); // else rad = 0
Matrix4x4 getTransformationMatrix(Matrix4x4 matrices[], int size);
Vector3D applyTransformation(Vector3D originVec, Matrix4x4 transform);

static Vector3D ORIGIN_3D = {0, 0, 0};

float dot(Vector3D *a, Vector3D *b);
float dot2D(Vector2D *a, Vector2D *b);
Vector3D *add(Vector3D *a, Vector3D *b, Vector3D *out);
Vector3D *sub(Vector3D *a, Vector3D *b, Vector3D *out);
Vector3D *mul(Vector3D *v, float f, Vector3D *out);
Vector3D *divide(Vector3D *v, float f, Vector3D *out);
Vector3D *cross(Vector3D *a, Vector3D *b, Vector3D *out);
Vector3D *norm(Vector3D *v, Vector3D *out);
float mag(Vector3D *v);
float mag2D(Vector2D *v);
float dist(Vector3D *a, Vector3D *b);
Intersection3D *intersect(Ray3D *r, Triangle3D *t, Intersection3D *out);
