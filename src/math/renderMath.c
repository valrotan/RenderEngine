#include <math.h>
#include "renderMath.h"

float dot(Vector *a, Vector *b) { // dot a, b; place result in out
	return a->dx * b->dx + a->dy * b->dy + a->dz * b->dz;
}

float mag(Vector *v) {
	return sqrtf(powf(v->dx, 2) + powf(v->dy, 2) + powf(v->dz, 2));
}
