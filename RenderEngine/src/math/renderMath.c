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

Vector3D *norm(Vector3D *v) {
	float l = mag(v);
	Vector3D *o = (Vector3D *)malloc(sizeof(Vector3D));
	o->x = v->x / l;
	o->y = v->y / l;
	o->z = v->z / l;
	return o;
}

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

Vector3D *divide(Vector3D *v, float f) {
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

// TODO: reflection + refraction + ...
Intersection3D *intersect(Ray3D *r, Triangle3D *t) {
	Intersection3D *i = (Intersection3D *)malloc(sizeof(Intersection3D));
	i->triangle = t;
	i->originalRay = r;

	// TODO: free all the intermediate vectors or
	// figure out how to do it no-copy style
	float u = -(dot(r->p, t->plane->v) + t->plane->d) / (dot(r->v, t->plane->v));
	i->point = add(r->p, mul(r->v, u));
	//	printf("%f %f %f\n", r->v.x, r->v.y, r->v.z);

	// out of bounds check for ray within triangle
	Vector3D *v1 = sub(t->p1, r->p);
	Vector3D *v2 = sub(t->p2, r->p);
	Vector3D *v3 = sub(t->p3, r->p);
	Vector3D *n1 = cross(v3, v1);
	Vector3D *n2 = cross(v1, v2);
	Vector3D *n3 = cross(v2, v3);
	//	norm(n1);
	//	norm(n2);
	//	norm(n3);
	float o1 = dot(i->point, n1);
	float o2 = dot(i->point, n2);
	float o3 = dot(i->point, n3);
	float d1 = dot(r->p, n1);
	float d2 = dot(r->p, n2);
	float d3 = dot(r->p, n3);
	if (o1 < d1) {
		i->point = 0;
	} else if (o2 < d2) {
		i->point = 0;
	} else if (o3 < d3) {
		i->point = 0;
	}
	free(v1);
	free(v2);
	free(v3);
	free(n1);
	free(n2);
	free(n3);

	return i;
}
