#include "renderMath.h"
#include <math.h>
#include <string.h>

double dot(Vector3D *a, Vector3D *b) {
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

double mag(Vector3D *v) {
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

double dot2D(Vector2D *a, Vector2D *b) { return a->x * b->x + a->y * b->y; }

double mag2D(Vector2D *v) { return sqrt(v->x * v->x + v->y * v->y); }

// Normalizes vector
// Pre: v - vector to be normalized
//		out - values of the vector are normalized so that the magnitude is 1
// Post: normilizes vector v
// Returns: result
Vector3D *norm(Vector3D *v, Vector3D *out) {
	double l = mag(v);
	out->x = v->x / l;
	out->y = v->y / l;
	out->z = v->z / l;
	return out;
}

// Adds vector a + vector b
// Pre: a - first vector
//		b - second vector
//		out - where the results of addition will be saved
// Post: vector a + vector b
// Returns: result
Vector3D *add(Vector3D *a, Vector3D *b, Vector3D *out) {
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;

	return out;
}

// Subtracts vector a - vector b
// Pre: a - first vector to subtract from
//		b - second vector that will be used to subtract from a
//		out - where the results will be saved
// Post: vector a - vector b
// Returns: result
Vector3D *sub(Vector3D *a, Vector3D *b, Vector3D *out) {

	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;

	return out;
}

Vector3D *mul(Vector3D *v, double f, Vector3D *out) {

	out->x = v->x * f;
	out->y = v->y * f;
	out->z = v->z * f;

	return out;
}

Vector3D *divide(Vector3D *v, double f, Vector3D *out) {

	out->x = v->x / f;
	out->y = v->y / f;
	out->z = v->z / f;

	return out;
}

// not safe to have out = a
Vector3D *cross(Vector3D *a, Vector3D *b, Vector3D *out) {

	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;

	return out;
}

double dist(Vector3D *a, Vector3D *b) {
	double dx = (b->x - a->x);
	double dy = (b->y - a->y);
	double dz = (b->z - a->z);
	return sqrt(dx * dx + dy * dy + dz * dz);
}

double distSqrd(Vector3D *a, Vector3D *b) {
	double dx = (b->x - a->x);
	double dy = (b->y - a->y);
	double dz = (b->z - a->z);
	return dx * dx + dy * dy + dz * dz;
}

// Finds an intersection between the given ray and the triangle
// Sets i->exists to 0 if intersection is not found or does not exist
// ~2300000 calls with BVH
// ~9400000 calls without BVH
Intersection3D *intersect(Ray3D *r, Triangle3D *t, Intersection3D *i) {
	//	static int count = 0;
	//	count ++;
	//	if (count % 100000 == 0) {
	//		printf("%d \n", count);
	//	}

	// Using Ray : P = P0 + Vu
	// and Plane : P * N + d = 0
	// we get    : u = -(P0 . N + d) / (V . N)
	// Intersection point is therefore :
	// P = P0 + Vu
	double u = -(dot(&r->p, &t->plane.v) + t->plane.d) / dot(&r->v, &t->plane.v);
	if (isinf(u) || isnan(u) ||
			u == 0.0 || // u == 0.0 prevents intersection at exactly camera point
			u < 0) {     // do not intersect behind ray origin
		i->exists = 0;
		return i;
	}
	Vector3D cameraToInter;
	add(&r->p, mul(&r->v, u, &cameraToInter), &i->point);

	// check if inside triangle bounds
	Vector3D v1;
	sub(&t->p1, &r->p, &v1);
	Vector3D v2;
	sub(&t->p2, &r->p, &v2);
	Vector3D v3;
	sub(&t->p3, &r->p, &v3);
	Vector3D n1;
	cross(&v3, &v1, &n1);
	Vector3D n2;
	cross(&v1, &v2, &n2);
	Vector3D n3;
	cross(&v2, &v3, &n3);

	double o1 = dot(&cameraToInter, &n1);
	double o2 = dot(&cameraToInter, &n2);
	double o3 = dot(&cameraToInter, &n3);

	int c = 0;
	if (o1 < 0) {
		c++;
	}
	if (o2 < 0) {
		c++;
	}
	if (o3 < 0) {
		c++;
	}
	if (c == 1 || c == 2) {
		i->exists = 0;
		return i;
	}

	i->exists = 1;
	return i;
}

Matrix4x4 getTranslationMatrix(double x, double y, double z) {
	Matrix4x4 translate;
	double t[4][4] = {{1, 0, 0, x}, //
									 {0, 1, 0, y}, //
									 {0, 0, 1, z}, //
									 {0, 0, 0, 1}};
	memcpy(translate.matrix, t, sizeof(t));
	return translate;
}

Matrix4x4 getScaleMatrix(double xScale, double yScale, double zScale) {
	Matrix4x4 scale;

	double s[4][4] = {{xScale, 0, 0, 0}, //
									 {0, yScale, 0, 0}, //
									 {0, 0, zScale, 0}, //
									 {0, 0, 0, 1}};

	memcpy(scale.matrix, s, sizeof(s));
	return scale;
}

double getRad(double angle) {
	angle = angle * PI / 180;
	return angle;
}

Matrix4x4 getXRotationMatrix(double angle, int rad) {
	if (rad != 1)
		angle = getRad(angle);

	Matrix4x4 rotateX;
	double x[4][4] = {{1, 0, 0, 0},                      //
									 {0, cos(angle), -sin(angle), 0}, //
									 {0, sin(angle), cos(angle), 0},  //
									 {0, 0, 0, 1}};
	memcpy(rotateX.matrix, x, sizeof(x));

	return rotateX;
}

Matrix4x4 getYRotationMatrix(double angle, int rad) {
	if (rad != 1)
		angle = getRad(angle);

	Matrix4x4 rotateY;

	double temp[4][4] = {{cos(angle), 0, sin(angle), 0},  //
											{0, 1, 0, 0},                      //
											{-sin(angle), 0, cos(angle), 0}, //
											{0, 0, 0, 1}};

	// assert(sizeof(temp) == sizeof(rotateY.matrix));
	memcpy(rotateY.matrix, temp, sizeof(temp));

	return rotateY;
}

Matrix4x4 getZRotationMatrix(double angle, int rad) {
	if (rad != 1)
		angle = getRad(angle);

	Matrix4x4 rotateZ;

	double temp[4][4] = {{cos(angle), -sin(angle), 0, 0}, //
											{sin(angle), cos(angle), 0, 0},  //
											{0, 0, 1, 0},                      //
											{0, 0, 0, 1}};

	// assert(sizeof(temp) == sizeof(rotateZ.matrix));
	memcpy(rotateZ.matrix, temp, sizeof(temp));

	return rotateZ;
}

Matrix4x4 getEye() {
	Matrix4x4 eye;

	double temp[4][4] = {{1, 0, 0, 0}, //
											{0, 1, 0, 0}, //
											{0, 0, 1, 0}, //
											{0, 0, 0, 1}};

	memcpy(eye.matrix, temp, sizeof(temp));

	return eye;
}

Matrix4x4 getTransformationMatrix(Matrix4x4 matrices[], int size) {
	Matrix4x4 result = matrices[0];
	for (int i = 1; i < size; i++) {
		result = multiply4x4Matrices(result, matrices[i]);
	}
	return result;
}

Vector3D *applyTransformation(Vector3D *orig, Matrix4x4 *transform,
															Vector3D *out) {
	double pnt[4] = {orig->x, orig->y, orig->z, 1};
	double r = 0;
	for (int j = 0; j < 4; j++) {
		r += transform->matrix[0][j] * pnt[j];
	}
	out->x = r;
	r = 0;
	for (int j = 0; j < 4; j++) {
		r += transform->matrix[1][j] * pnt[j];
	}
	out->y = r;
	r = 0;
	for (int j = 0; j < 4; j++) {
		r += transform->matrix[2][j] * pnt[j];
	}
	out->z = r;
	return out;
}

Matrix4x4 multiply4x4Matrices(Matrix4x4 a, Matrix4x4 b) {
	Matrix4x4 result;

	/* Pseudo:
		Go through each row in a
			stay on that row and mult all elements by all elements in all cols in b
			after each col add result to result matrix
		after done -> start adding to the next row + switch result row
	*/
	for (int i = 0; i < 4; i++) {   // row in matrix a
		for (int j = 0; j < 4; j++) { // current column in b
			double r = 0;
			for (int k = 0; k < 4; k++) { // changin cols in a and rows in b
				r += a.matrix[i][k] * b.matrix[k][j];
			}
			result.matrix[i][j] = r;
		}
	}

	return result;
}

void setMinMax(const double *point, double *min, double *max) {
	*min = *point < *min ? *point : *min;
	*max = *point > *max ? *point : *max;
}

Vector3D *triangleGetX(Triangle3D *t, Vector3D *v) {
	v->x = t->p1.x;
	v->y = t->p2.x;
	v->z = t->p3.x;
	return v;
}

Vector3D *triangleGetY(Triangle3D *t, Vector3D *v) {
	v->x = t->p1.y;
	v->y = t->p2.y;
	v->z = t->p3.y;
	return v;
}

Vector3D *triangleGetZ(Triangle3D *t, Vector3D *v) {
	v->x = t->p1.z;
	v->y = t->p2.z;
	v->z = t->p3.z;
	return v;
}

double dotOne(Vector3D *v) { return v->x + v->y + v->z; }

double triangleCentroidX(Triangle3D *t) { return t->centroid.x; }
double triangleCentroidY(Triangle3D *t) { return t->centroid.y; }
double triangleCentroidZ(Triangle3D *t) { return t->centroid.z; }

double sigmoid(double x) {
	double ex = pow(E, x);
	return ex / (ex + 1);
}
