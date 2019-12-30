#include "renderMath.h"
#include "stdlib.h"
#include <math.h>

float dot(Vector3D *a, Vector3D *b) { // dot a, b; place result in out
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

float mag(Vector3D *v) {
	return sqrtf(powf(v->x, 2) + powf(v->y, 2) + powf(v->z, 2));
}

float dot2D(Vector2D *a, Vector2D *b) { // dot a, b; place result in out
	return a->x * b->x + a->y * b->y;
}

float mag2D(Vector2D *v) { return sqrtf(powf(v->x, 2) + powf(v->y, 2)); }

Vector3D *add(Vector3D *a, Vector3D *b) {
	Vector3D *v = (Vector3D *)malloc(sizeof(Vector3D));

	v->x = a->x + b->x;
	v->y = a->y + b->y;
	v->z = a->z + b->z;

	return v;
}

Vector3D *sub(Vector3D *a, Vector3D *b) {
	Vector3D *v = (Vector3D *)malloc(sizeof(Vector3D));

	v->x = a->x - b->x;
	v->y = a->y - b->y;
	v->z = a->z - b->z;

	return v;
}

Vector3D *mul(Vector3D *v, float f) {
	Vector3D *w = (Vector3D *)malloc(sizeof(Vector3D));

	w->x = v->x * f;
	w->y = v->y * f;
	w->z = v->z * f;

	return w;
}

Vector3D *div(Vector3D *v, float f) {
	Vector3D *w = (Vector3D *)malloc(sizeof(Vector3D));

	w->x = v->x / f;
	w->y = v->y / f;
	w->z = v->z / f;

	return w;
}

Vector3D *cross(Vector3D *a, Vector3D *b) {
	Vector3D *n = (Vector3D *)malloc(sizeof(Vector3D));

	n->x = a->y * b->z - a->z * b->y;
	n->y = a->z * b->x - a->x * b->z;
	n->z = a->x * b->y - a->y * b->x;

	return n;
}

float dist(Vector3D *a, Vector3D *b) {
	return sqrtf(powf(b->x - a->x, 2) + powf(b->x - a->y, 2) +
							 powf(b->z - a->z, 2));
}

// TODO: experiment with out of bounds
// TODO: reflection + refraction
// TODO: get rid of copy for intersection point setting
Intersection3D *intersect(Ray3D *r, Triangle3D *t) {
	Intersection3D *i = (Intersection3D *)malloc(sizeof(Intersection3D));
	i->triangle = t;
	i->originalRay = r;

	float u = -(dot(&(r->p), &(t->plane.v)) + t->plane.d) /
						(dot(&(r->v), &(t->plane.v)));
	i->point = *add(&(r->p), mul(&(r->v), u));

	// TODO: out of bounds check for ray within triangle

	return i;
}
