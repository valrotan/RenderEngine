#include "renderMathTests.h"
#include <math.h>
#include <sys/timeb.h>

// TODO: don't compare floats directly
void vectorTests() {
	Vector3D a = {1, 2, 3};
	Vector3D b = {4, 5, 6};

	assertTrue(dot(&a, &b) == 32.0f, "dot test 1");
	a.x = -1;
	a.y = -2;
	a.z = -3;
	assertTrue(-dot(&a, &b) == 32.0f, "dot test 2");
	b.x = -4;
	b.y = -5;
	b.z = -6;
	assertTrue(dot(&a, &b) == 32.0f, "dot test 3");
	a.x = -1.5;
	assertTrue(dot(&a, &b) == 34.0f, "dot test 4");
}

void intersectTests() {

	Vector3D a = {-1, -1, -1};
	Vector3D b = {-1, 1, -1};
	Vector3D c = {1, 0, -1};
	Triangle3D triangle;
	triangle.p1 = b;
	triangle.p2 = a;
	triangle.p3 = c;
	Plane3D plane;
	Vector3D n;
	Vector3D temp1;
	Vector3D temp2;
	norm(cross(sub(&b, &a, &temp1), sub(&c, &a, &temp2), &n), &n);
	plane.v = n;
	plane.d = -dot(&b, mul(&n, -1, &n));
	triangle.plane = plane;
	Intersection3D inter;
	Vector3D rayDir = {0, 0, -1};
	Ray3D ray;
	ray.p = ORIGIN_3D;
	ray.v = rayDir;

	intersect(&ray, &triangle, &inter);

	printf("Triangle: \n");
	printf("  Normal (%.2f, %.2f, %.2f) \n", n.x, n.y, n.z);
	printf("Inter: %d \n", inter.exists);
	printf("  Point  (%.2f, %.2f, %.2f) \n", inter.point.x, inter.point.y,
				 inter.point.z);
}

void distSpeedTests(void);

void runMathTests() {
	vectorTests();
	intersectTests();
	distSpeedTests();
}

float dista(Vector3D *a, Vector3D *b) {
	return sqrtf(powf(b->x - a->x, 2) + powf(b->y - a->y, 2) +
							 powf(b->z - a->z, 2));
}

float distb(Vector3D *a, Vector3D *b) {
	float dx = (b->x - a->x);
	float dy = (b->y - a->y);
	float dz = (b->z - a->z);
	return sqrtf(dx * dx + dy * dy + dz * dz);
}

float distSa(Vector3D *a, Vector3D *b) {
	return powf(b->x - a->x, 2) + powf(b->y - a->y, 2) + powf(b->z - a->z, 2);
}

float distSb(Vector3D *a, Vector3D *b) {
	float dx = (b->x - a->x);
	float dy = (b->y - a->y);
	float dz = (b->z - a->z);
	return dx * dx + dy * dy + dz * dz;
}

void distSpeedTests() {
	Vector3D a, b;
	struct timeb start, end;
	int diff;

	ftime(&start);

	for (int i = 0; i < 10000000; i++) {
		a.x = i / 11;
		a.y = -2 * i / 11;
		a.z = 3 * i / 11;
		b.x = -i / 13;
		b.y = 2 * i / 13;
		b.z = -3 * i / 13;

		dista(&a, &b);
	}

	ftime(&end);
	diff = (int)(1000.0 * (end.time - start.time) + //
							 (end.millitm - start.millitm));
	printf("dista took %u milliseconds \n", diff);

	ftime(&start);

	for (int i = 0; i < 10000000; i++) {
		a.x = i / 11;
		a.y = -2 * i / 11;
		a.z = 3 * i / 11;
		b.x = -i / 13;
		b.y = 2 * i / 13;
		b.z = -3 * i / 13;

		distb(&a, &b);
	}

	ftime(&end);
	diff = (int)(1000.0 * (end.time - start.time) + //
							 (end.millitm - start.millitm));
	printf("distb took %u milliseconds \n", diff);

	ftime(&start);

	for (int i = 0; i < 10000000; i++) {
		a.x = i / 11;
		a.y = -2 * i / 11;
		a.z = 3 * i / 11;
		b.x = -i / 13;
		b.y = 2 * i / 13;
		b.z = -3 * i / 13;

		distSa(&a, &b);
	}

	ftime(&end);
	diff = (int)(1000.0 * (end.time - start.time) + //
							 (end.millitm - start.millitm));
	printf("distSa took %u milliseconds \n", diff);

	ftime(&start);

	for (int i = 0; i < 10000000; i++) {
		a.x = i / 11;
		a.y = -2 * i / 11;
		a.z = 3 * i / 11;
		b.x = -i / 13;
		b.y = 2 * i / 13;
		b.z = -3 * i / 13;

		distSb(&a, &b);
	}

	ftime(&end);
	diff = (int)(1000.0 * (end.time - start.time) + //
							 (end.millitm - start.millitm));
	printf("distSb took %u milliseconds \n", diff);
}

// performance tests with 1/f
Vector3D *dividea(Vector3D *v, float f, Vector3D *out) {

	out->x = v->x / f;
	out->y = v->y / f;
	out->z = v->z / f;

	return out;
}
