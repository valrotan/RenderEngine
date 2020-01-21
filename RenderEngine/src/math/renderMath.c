#include "renderMath.h"
#include <stdlib.h>
#include <math.h>

float dot(Vector3D *a, Vector3D *b) {
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

float mag(Vector3D *v) {
	return sqrtf(powf(v->x, 2) + powf(v->y, 2) + powf(v->z, 2));
}

float dot2D(Vector2D *a, Vector2D *b) {
	return a->x * b->x + a->y * b->y;
}

float mag2D(Vector2D *v) { return sqrtf(powf(v->x, 2) + powf(v->y, 2)); }

Vector3D *norm(Vector3D *v, Vector3D *out) {
	float l = mag(v);
	out->x = v->x / l;
	out->y = v->y / l;
	out->z = v->z / l;
	return out;
}

Vector3D *add(Vector3D *a, Vector3D *b, Vector3D *out) {

	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;

	return out;
}

Vector3D *sub(Vector3D *a, Vector3D *b, Vector3D *out) {

	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;

	return out;
}

Vector3D *mul(Vector3D *v, float f, Vector3D *out) {

	out->x = v->x * f;
	out->y = v->y * f;
	out->z = v->z * f;

	return out;
}

Vector3D *divide(Vector3D *v, float f, Vector3D *out) {

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

float dist(Vector3D *a, Vector3D *b) {
	return sqrtf(powf(b->x - a->x, 2) + powf(b->x - a->y, 2) +
							 powf(b->z - a->z, 2));
}

// TODO: reflection + refraction + ...
Intersection3D *intersect(Ray3D *r, Triangle3D *t, Intersection3D *i) {
	i->triangle = t;
	i->originalRay = r;

	float denom = dot(r->v, t->plane->v);
	float u = -(dot(r->p, t->plane->v) + t->plane->d) / denom;
	if (isinf(u) || isnan(u) ||
			u == 0.0f) { // u == 0.0f prevents intersection at exactly camera point
									 // (when y = 0 for all 3 pts)
		i->exists = 0;
		return i;
	}
	add(r->p, mul(r->v, u, i->point), i->point);

	// out of bounds check for ray within triangle
	Vector3D v1;
	sub(t->p1, r->p, &v1);
	Vector3D v2;
	sub(t->p2, r->p, &v2);
	Vector3D v3;
	sub(t->p3, r->p, &v3);
	Vector3D n1;
	cross(&v3, &v1, &n1);
	Vector3D n2;
	cross(&v1, &v2, &n2);
	Vector3D n3;
	cross(&v2, &v3, &n3);

	float o1 = dot(i->point, &n1);
	float o2 = dot(i->point, &n2);
	float o3 = dot(i->point, &n3);
	float d1 = dot(r->p, &n1);
	float d2 = dot(r->p, &n2);
	float d3 = dot(r->p, &n3);

//	printf("%.2f %.2f ; %.2f %.2f ; %.2f %.2f \n", o1, d1, o2, d2, o3, d3);

	int c = 0;
	if (o1 < d1) {
		i->exists = 0;
	}
	if (o2 < d2) {
		i->exists = 0;
	}
	if (o3 < d3) {
		i->exists = 0;
	}

	return i;
}


Matrix4x4 getTranslationMatrix(float x, float y, float z) {
	Matrix4x4 translate;
	float t[4][4]= {{1,0,0,x},
					{0,1,0,y},
					{0,0,1,z},
					{0,0,0,1}};
	memcpy(translate.matrix, t, sizeof(t));
	return translate;
}

Matrix4x4 getScaleMatrix(float xScale, float yScale, float zScale) {
	Matrix4x4 scale; 
	
	float s[4][4] = {{xScale,0,0,0},
					{0,yScale,0,0},
					{0,0,zScale,0},
					{0,0,0,1}};

	memcpy(scale.matrix, s, sizeof(s));
	return scale;
}

float getRad(float angle) {
	const float PI = 3.14159265359;
	angle = angle * PI / 180;
	return angle;
}

Matrix4x4 getXRotationMatrix(float angle, int rad) {
	if (rad != 1)
		angle = getRad(angle);

	Matrix4x4 rotateX;
	float x[4][4]= {
						{1,0,0,0},
						{0,cos(angle),-sin(angle),0},
						{0,sin(angle),cos(angle),0},
						{0,0,0,1}};
	memcpy(rotateX.matrix, x, sizeof(x));

	return rotateX;
}

Matrix4x4 getYRotationMatrix(float angle, int rad) {
	if (rad != 1)
		angle = getRad(angle);

	Matrix4x4 rotateY;

	float temp[4][4] = {{cos(angle), 0, sin(angle), 0},
						{0, 1, 0, 0},
						{-sin(angle), 0, cos(angle), 0},
						{0,0,0,1}};

	//assert(sizeof(temp) == sizeof(rotateY.matrix));
	memcpy(rotateY.matrix, temp, sizeof(temp));

	return rotateY;
}

Matrix4x4 getZRotationMatrix(float angle, int rad) {
	if (rad != 1)
		angle = getRad(angle);
	
	Matrix4x4 rotateZ;

	float temp[4][4] = { {cos(angle), -sin(angle), 0, 0},
						{sin(angle), cos(angle), 0, 0},
						{0,0,1,0},
						{0,0,0,1} };

	//assert(sizeof(temp) == sizeof(rotateZ.matrix));
	memcpy(rotateZ.matrix, temp, sizeof(temp));

	
	return rotateZ;
}

Matrix4x4 getEye() {
	Matrix4x4 eye;

	float temp[4][4] = {{1,0,0,0},
						{0,1,0,0},
						{0,0,1,0},
						{0,0,0,1}};

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

Vector3D applyTransformation(Vector3D orig, Matrix4x4 transform) {
	float pnt[4] = { orig.x, orig.y, orig.z, 1};
	Vector3D result = {0,0,0};
	float temp[4] = {0,0,0,0};
	for (int i = 0; i < 4; i++) {
		float r = 0;
		for (int j = 0; j < 4; j++) {
			r += transform.matrix[i][j] * pnt[j];
		}
		temp[i] = r;
	}
	result.x = temp[0];
	result.y = temp[1];
	result.z = temp[2];
	return result;
}

Matrix4x4 multiply4x4Matrices(Matrix4x4 a, Matrix4x4 b) {
	Matrix4x4 result;

	/* Pseudo:
		Go through each row in a
			stay on that row and mult all elements by all elements in all cols in b
			after each col add result to result matrix
		after done -> start adding to the next row + switch result row
	*/
	for (int i = 0; i < 4; i++) { // row in matrix a
 		for (int j = 0; j < 4; j++) { // current column in b
			float r = 0;
			for (int k = 0; k < 4; k++) { // changin cols in a and rows in b
				r += a.matrix[i][k] * b.matrix[k][j];
			}
			result.matrix[i][j] = r;
		}
	}

	return result;
}