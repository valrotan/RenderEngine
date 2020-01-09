#include "renderer/renderer.h"
#include "visualizer/visualizer.h"
#include <stdio.h>
#include <stdlib.h>

void triangleRayIntersectDemo() {
	printf("Starting render engine...\n");

	int WIDTH = 512, HEIGHT = 512;

	printf("Initializing visualizer...\n");
	visInit(WIDTH, HEIGHT);
	unsigned char *screen = visGetPixbuf();

	printf("Initializing renderer...\n");
	Camera camera;
	Vector3D pos = {0, 0, 150};
	Vector3D dir = {0, 0, -1};
	Vector3D up = {0, 1, 0};
	camera.pos = pos;
	camera.dir = dir;
	camera.up = up;
	camera.screenZ = 150;

	Scene scene;
	Vector3D a = {0, 0, 0};
	Vector3D b = {0, 75, 100};
	Vector3D c = {75, 0, 100};
	Vector3D d = {0, 0, 0};
	Vector3D e = {0, -75, 100};
	Vector3D f = {-75, 0, 100};
	Triangle3D *t = (Triangle3D *)malloc(sizeof(Triangle3D) * 2);
	Triangle3D temp1 = {&a, &b, &c, 0, .2};
	Triangle3D temp2 = {&d, &e, &f, 0, .2};
	t[0] = temp1;
	t[1] = temp2;
	scene.triangles = t;
	scene.nTriangles = 2;

	PointLight pointLight;
	Vector3D pointLightLoc = {0, 20, 150};
	pointLight.point = &pointLightLoc;
	pointLight.intensity = 2;
	Vector3D pointLightCoeffs = {0, 0, 0};
	pointLight.attenuationCoeffs = &pointLightCoeffs;
	scene.pointLights = &pointLight;
	scene.nPointLights = 1;

	Renderer renderer = {&camera, &scene};

	rendererInit(&renderer);

	printf("Raycasting...\n");

	rayCast(&camera, &scene, screen, WIDTH, HEIGHT);

	printf("Showing...\n");

	visShowStill();
}

// TODO: go through and free memory probably
int main() {
	triangleRayIntersectDemo();
	return 0;
}
