#include "renderMathTests.h"

// TODO: don't compare floats directly
void vectorTests() {
	Vector3D a = {1,2,3};
	Vector3D b = {4,5,6};

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
	triangle.p1 = &b;
	triangle.p2 = &a;
	triangle.p3 = &c;
	Plane3D plane;
	Vector3D n;
	Vector3D temp1;
	Vector3D temp2;
	norm(cross(sub(&b, &a, &temp1), sub(&c, &a, &temp2), &n), &n);
	plane.v = &n;
	plane.d = -dot(&b, mul(&n, -1, &n));
	triangle.plane = &plane;
	Intersection3D inter;
	Vector3D pointInter;
	inter.point = &pointInter;
	Vector3D rayDir = {0, 0, -1};
	Ray3D ray;
	ray.p = &ORIGIN_3D;
	ray.v = &rayDir;

	intersect(&ray, &triangle, &inter);

	printf("Triangle: \n");
	printf("  Normal (%.2f, %.2f, %.2f) \n", n.x,n.y, n.z);
	printf("Inter: %d \n", inter.exists);
	printf("  Point  (%.2f, %.2f, %.2f) \n", inter.point->x, inter.point->y, inter.point->z);
}

void runMathTests() {
	vectorTests();
	intersectTests();
}
