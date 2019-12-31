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

void runMathTests() {
	vectorTests();
}
