#include "renderMathTests.h"

// TODO: don't compare floats directly
void vectorTests() {
	Vector a = {1,2,3};
	Vector b = {4,5,6};
	assertTrue(dot(&a, &b) == 32.0f, "dot test 1");
	a.dx = -1;
	a.dy = -2;
	a.dz = -3;
	assertTrue(dot(&a, &b) == -32.0f, "dot test 2");
	b.dx = -4;
	b.dy = -5;
	b.dz = -6;
	assertTrue(dot(&a, &b) == 32.0f, "dot test 3");
	a.dx = -1.5;
	assertTrue(dot(&a, &b) == 34.0f, "dot test 4");
}

void runMathTests() {
	vectorTests();
}
