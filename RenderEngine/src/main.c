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
	Vector3D pos = {0, 0, 200};
	Vector3D dir = {0, 0, -1};
	Vector3D up = {0, 1, 0};
	camera.pos = pos;
	camera.dir = dir;
	camera.up = up;
	camera.screenZ = 450;

	Scene scene;
	scene.bkgR = 64;
	scene.bkgG = 64;
	scene.bkgB = 64;

	Triangle3D *t = (Triangle3D *)malloc(sizeof(Triangle3D) * 8);
	Vector3D a = {0, 50, 0};
	Vector3D b = {50, 100, -50};
	Vector3D c = {50, 50, -50};
	Vector3D d = {0, 50, 0};
	Vector3D e = {0, 100, 0};
	Vector3D f = {50, 100, -50};
	Triangle3D temp1 = {&a, &b, &c, 0, 255, 0, 0, .1f, .25f, .25f};
	Triangle3D temp2 = {&d, &e, &f, 0, 255, 0, 0, .1f, .25f, .25f};
	t[0] = temp1;
	t[1] = temp2;

	Vector3D ab = {0, 50, 0};
	Vector3D bb = {-50, 50, -50};
	Vector3D cb = {-50, 100, -50};
	Vector3D db = {0, 50, 0};
	Vector3D eb = {-50, 100, -50};
	Vector3D fb = {0, 100, 0};
	Triangle3D temp1b = {&ab, &bb, &cb, 0, 0, 255, 0, 0, .25f, .25f};
	Triangle3D temp2b = {&db, &eb, &fb, 0, 0, 255, 0, 0, .25f, .25f};
	t[2] = temp1b;
	t[3] = temp2b;

	Vector3D ac = {0, 50, 0};
	Vector3D bc = {-50, 50, -50};
	Vector3D cc = {0, 50, -100};
	Vector3D dc = {0, 50, 0};
	Vector3D ec = {0, 50, -100};
	Vector3D fc = {50, 50, -50};
	Triangle3D temp1c = {&ac, &bc, &cc, 0, 0, 0, 255, .75f, .75f, .25f};
	Triangle3D temp2c = {&dc, &ec, &fc, 0, 0, 0, 255, .75f, .75f, .25f};
	t[4] = temp1c;
	t[5] = temp2c;

	Vector3D ad = {0, 0, 0};
	Vector3D bd = {50, 30, 60};
	Vector3D cd = {50, -30, 60};
	Vector3D dd = {0, 0, 0};
	Vector3D ed = {-50, -50, 40};
	Vector3D fd = {-50, 50, 40};
	Triangle3D temp1d = {&ad, &bd, &cd, 0, 255, 0, 0, .5f, .5f, .25f};
	Triangle3D temp2d = {&dd, &ed, &fd, 0, 0, 0, 255, .5f, .5f, .25f};
	t[6] = temp1d;
	t[7] = temp2d;

	scene.triangles = t;
	scene.nTriangles = 8;

	PointLight pointLight;
	Vector3D pointLightLoc = {0, 0, 50};
	pointLight.point = &pointLightLoc;
	pointLight.intensity = 5;
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
