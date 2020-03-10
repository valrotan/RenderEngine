#include "imageUtil/imageUtil.h"
#include "parsing/objParser.h"
#include "renderer/renderer.h"
#include "visualizer/visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

float scale = 1; // remove sometime
int iter = 0;

void render(unsigned char *screen, void *voidRenderer) {
	Renderer *renderer = (Renderer *)voidRenderer;

	Matrix4x4 trans[] = {getScaleMatrix(scale, scale, scale), //
											 getYRotationMatrix(30 * iter++, 0),   //
											 getXRotationMatrix(-25, 0),          //
											 getTranslationMatrix(0, .1f, 2.1f)};
	renderer->camera->cameraToWorld = getTransformationMatrix(trans, 4);

	rendererInit(renderer);
	rayTrace(renderer, screen);
	printf("%d \n", iter);
}

int main(int argc, char **argv) {

	printf("Starting render engine...\n");

	int WIDTH = 1280, HEIGHT = 720;

	// increase x to scroll right, decrease x to scroll left
	// increase y to scroll up, decrease to scroll down
	// increase z to zoom out, decrease z to zoom in

	// ORIGINAL AXIS:
	// x-axis - left to right
	// y-axis - top to bottom
	// z-axis - directed through the camera

	printf("Initializing renderer...\n");

	Triangle3D *t;
	int size;

	// FinalBaseMesh.obj
	// tea.obj 17
	// tinker.obj 420

	char *path = "RenderEngine/input/torus.obj";
	if (argc > 1) {
		path = argv[1];
	}
	scale = 1;

	parseObj(path, &t, &size, &scale);

	Camera camera;
	Matrix4x4 trans[] = {getScaleMatrix(scale, scale, scale), //
											 getYRotationMatrix(0, 0),            //
											 getXRotationMatrix(-180, 0),          //
											 getTranslationMatrix(0, 0, 2.5f)};
	Matrix4x4 camToWorld = getTransformationMatrix(trans, 4);

	camera.width = WIDTH;
	camera.height = HEIGHT;
	camera.fov = 60;
	camera.cameraToWorld = camToWorld;

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
	Vector3D dirLightDir0 = {-1, -1, 1};
	dirLights[0].direction = norm(&dirLightDir0, &dirLightDir0);
	dirLights[0].intensity = .65f;
	Vector3D dirLightDir1 = {1, -1, -1};
	dirLights[1].direction = norm(&dirLightDir1, &dirLightDir1);
	dirLights[1].intensity = .65f;
	Vector3D dirLightDir2 = {0, -1, 1};
	dirLights[2].direction = norm(&dirLightDir2, &dirLightDir2);
	dirLights[2].intensity = .65f;

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
	renderer.nThreads = 192;
	renderer.nTraces = 0;

	printf("Initializing visualizer...\n");

	Visualizer vis;
	int loadVideo = 0;
	// CHANGE THE FPS VALUE IN VISUALIZER IF THE MODEL TAKES MORE THAN 100 MS TO
	// LOAD

	if (loadVideo) {
		visInitVideo(&vis, WIDTH, HEIGHT, &render, &renderer);
		visShowVideo(&vis);
	} else {
		printf("Raycasting...\n");
		visInit(&vis, WIDTH, HEIGHT);

		struct timeb start, end;
		int diff;
		ftime(&start);

		rendererInit(&renderer);
		rayTrace(&renderer, vis.pixels);

		ftime(&end);
		diff =
				(int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
		printf("Render took %u milliseconds \n", diff);

		printf("Showing...\n");
		visShowStill(&vis);
	}

	//	printf("%d \n", saveToTGA("image.tga", screen, WIDTH, HEIGHT));

	//	visShowStill();

	return 0;
}
