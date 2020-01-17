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

	// TODO: free all the intermediate vectors or
	// figure out how to do it no-copy style
	//  float num = (dot(r->p, t->plane->v) + t->plane->d);
	//  float den = dot(r->v, t->plane->v);
	float denom = dot(r->v, t->plane->v);
	float u = -(dot(r->p, t->plane->v) + t->plane->d) / denom;
	if (isinf(u) || isnan(u) ||
			u == 0.0f) { // u == 0.0f prevents intersection at exactly camera point
									 // (when y = 0 for all 3 pts)
		i->exists = 0;
		return i;
	}
	Vector3D temp;
	add(r->p, mul(r->v, u, &temp), i->point);
//	if (fabsf(dot(i->point, t->plane->v) + t->plane->d) > .0001f) {
//		//	return 0;
//	}

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

//	printf("inter 3 \n");

	float o1 = dot(i->point, &n1);
	float o2 = dot(i->point, &n2);
	float o3 = dot(i->point, &n3);
	float d1 = dot(r->p, &n1);
	float d2 = dot(r->p, &n2);
	float d3 = dot(r->p, &n3);
	if (o1 < d1) {
		i->exists = 0;
	} else if (o2 < d2) {
		i->exists = 0;
	} else if (o3 < d3) {
		i->exists = 0;
	}

	return i;
}
