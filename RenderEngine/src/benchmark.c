#include "renderer/renderer.h"
#include "visualizer/visualizer.h"
#include "parsing/objParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "imageUtil/imageUtil.h"

void rayTraceDemo() {
	printf("Starting render engine...\n");

	int WIDTH = 1280, HEIGHT = 720;

	printf("Initializing visualizer...\n");
	visInit(WIDTH, HEIGHT);
	unsigned char *screen = visGetPixbuf();

	printf("Initializing renderer...\n");
	Camera camera;
	Matrix4x4 trans[] = {getXRotationMatrix(-45, 0), getScaleMatrix(0.2f,0.2f,0.2f),
											 getTranslationMatrix(0, 10, 40)};
	Matrix4x4 camToWorld = getTransformationMatrix(trans, 3);

	camera.width = WIDTH;
	camera.height = HEIGHT;
	camera.fov = 60;
	camera.cameraToWorld = camToWorld;

	Triangle3D *t;
	int size;
	parseObj("RenderEngine/input/tea.obj", &t, &size);

	Scene scene;
	scene.bkgR = .3f;
	scene.bkgG = .3f;
	scene.bkgB = .3f;
	scene.ambientLight = .25f;
	scene.kSpecularExponent = 3;

	scene.triangles = t;
	scene.nTriangles = size;

	PointLight *pointLights = (PointLight *)malloc(sizeof(PointLight));
	Vector3D pointLightLoc1 = {0, -40, -50};
	pointLights[0].point = &pointLightLoc1;
	pointLights[0].intensity = 20;
	Vector3D pointLightCoeffs1 = {2, .01f, .005f};
	pointLights[0].attenuationCoeffs = &pointLightCoeffs1;

	scene.pointLights = pointLights;

	DirectionalLight dirLights[4];
	Vector3D dirLightDir0 = {-1, -1, -1};
	dirLights[0].direction = norm(&dirLightDir0, &dirLightDir0);
	dirLights[0].intensity = 1;

	scene.directionalLights = dirLights;

	SpotLight spotLights[4];
	Vector3D spotLightLoc0 = {-60, -20, 100};
	Vector3D spotLightDir0 = {1, -.25f, 0};
	Vector3D spotLightCoeffs0 = {1, 1, 3.0f};
	spotLights[0].point = &spotLightLoc0;
	spotLights[0].direction = norm(&spotLightDir0, &spotLightDir0);
	spotLights[0].attenuationCoeffs = &spotLightCoeffs0;
	spotLights[0].intensity = 2000.0f;

	Vector3D spotLightLoc1 = {60, -20, 100};
	Vector3D spotLightDir1 = {-1, -.25f, 0};
	Vector3D spotLightCoeffs1 = {1, 1, 3.0f};
	spotLights[1].point = &spotLightLoc1;
	spotLights[1].direction = norm(&spotLightDir1, &spotLightDir1);
	spotLights[1].attenuationCoeffs = &spotLightCoeffs1;
	spotLights[1].intensity = 2000.0f;

	scene.spotLights = spotLights;

	// *** light counts
	scene.nPointLights = 0;
	scene.nDirectionalLights = 1;
	scene.nSpotLights = 0;

	Renderer renderer = {&camera, &scene, 0};
	renderer.nThreads = 128;

	printf("Raycasting...\n");

	struct timeb start, end;
	int diff;
	ftime(&start);

	rendererInit(&renderer);
	rayTrace(&renderer, screen);
	ftime(&end);
	diff =
			(int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));

	printf("Render took %u milliseconds \n", diff);

	printf("Writing...\n");

	printf("%d \n", saveToTGA("image.tga", screen, WIDTH, HEIGHT));

	visShowStill();
}

int main() {
	rayTraceDemo();

	return 0;
}
