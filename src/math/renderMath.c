#include <math.h>
#include "renderMath.h"

float dot(Vector3D *a, Vector3D *b) { // dot a, b; place result in out
	return a->dx * b->dx + a->dy * b->dy + a->dz * b->dz;
}

float mag(Vector3D *v) {
	return sqrtf(powf(v->dx, 2) + powf(v->dy, 2) + powf(v->dz, 2));
}

float dot2D(Vector2D *a, Vector2D *b) { // dot a, b; place result in out
	return a->dx * b->dx + a->dy * b->dy;
}

float mag2D(Vector2D *v) {
	return sqrtf(powf(v->dx, 2) + powf(v->dy, 2));
}
