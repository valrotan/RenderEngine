#include "rendererTests.h"
#include <sys/timeb.h>

void runPerformanceTests(void);

// TODO: come up with tests
void runRendererTests() {
	runPerformanceTests();
}

// MBP performance: ~50ms for HD on single thread
void rayThroughPixelPerformance() {

	int WIDTH = 1280, HEIGHT = 720;

	Camera camera;
	Matrix4x4 trans[] = {getXRotationMatrix(0, 0),
											 getTranslationMatrix(0, -20, 170)};
	Matrix4x4 camToWorld = getTransformationMatrix(trans, 2);

	camera.width = WIDTH;
	camera.height = HEIGHT;
	camera.fov = 60;
	camera.cameraToWorld = camToWorld;

	Ray3D ray;
	Vector3D rayp, rayv;
	ray.p = &rayp;
	ray.v = &rayv;

	struct timeb start, end;
	int diff;
	ftime(&start);

	applyTransformation(&ORIGIN_3D, &camera.cameraToWorld, ray.p);
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			constructRayThroughPixel(&camera, x, y, WIDTH, HEIGHT, &ray);
		}
	}

	ftime(&end);
	diff =
			(int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));

	printf("Ray through pixel performance test \n");
	printf("  Resolution:              %dx%x \n", WIDTH, HEIGHT);
	printf("  Total rays constructed:  %d \n", WIDTH * HEIGHT);
	printf("  Total time:              %d ms \n", diff);
}

void runPerformanceTests() {
 rayThroughPixelPerformance();
}
