#include "imageUtil/imageUtil.h"
#include "parsing/objParser.h"
#include "renderer/renderer.h"
#include "visualizer/visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

int main(int argc, char **argv) {

	printf("Starting render engine...\n");

	int WIDTH = 1280, HEIGHT = 720;

	printf("Initializing visualizer...\n");
	visInit(WIDTH, HEIGHT);
	unsigned char *screen = visGetPixbuf();

	printf("Initializing renderer...\n");
	Camera camera;
	Matrix4x4 trans[] = {getXRotationMatrix(-30, 0), getTranslationMatrix(0, 1.25, 6)};
	Matrix4x4 camToWorld = getTransformationMatrix(trans, 2);

	camera.width = WIDTH;
	camera.height = HEIGHT;
	camera.fov = 60;
	camera.cameraToWorld = camToWorld;

	Triangle3D *t;
	int size;
	char *path = "RenderEngine/input/FinalBaseMesh.obj";
	if (argc > 1) {
		path = argv[1];
	}
	parseObj(path, &t, &size);

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
	dirLights[0].intensity = .5;
	Vector3D dirLightDir1 = {1, -1, -1};
	dirLights[1].direction = norm(&dirLightDir1, &dirLightDir1);
	dirLights[1].intensity = .5;
	Vector3D dirLightDir2 = {0, -1, 1};
	dirLights[2].direction = norm(&dirLightDir2, &dirLightDir2);
	dirLights[2].intensity = .5;

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
	scene.nDirectionalLights = 3;
	scene.nSpotLights = 0;

	Renderer renderer = {&camera, &scene, 0, 0};
	renderer.nThreads = 128;
	renderer.nTraces = 3;

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

	return 0;
}
