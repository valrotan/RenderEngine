#include "engine.h"

#include "imageUtil/imageUtil.h"
#include "parsing/objParser.h"
#include "renderer/renderer.h"
#include "visualizer/visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

RenderArgs engineDefaultArgs() {
	RenderArgs args = {0};

	args.resolution.x = 1280;
	args.resolution.y = 720;
	args.objPath = "./RenderEngine/input/zero_blue.obj";
	args.outputPath = "output.tga";

	args.fov = 60;
	args.scale = 1;

	args.bkgColor.x = .4;
	args.bkgColor.y = .4;
	args.bkgColor.z = .4;
	args.ambientLight = .15;
	args.kSpecularExponent = 3;

	args.nAntialiasingSamples = 2;
	args.nTraces = 2;
	args.nThreads = 96;

	args.loadVideo = 0;

	args.baseTriangle.k_e = .3;
	args.baseTriangle.k_d = .3;
	args.baseTriangle.k_s = .5;

	return args;
}

// double scale = 1; // remove sometime
// int iter = 0;

// void render(unsigned char *screen, void *voidRenderer) {
//	Renderer *renderer = (Renderer *)voidRenderer;

//	Matrix4x4 trans[] = {getScaleMatrix(scale, scale, scale), //
//											 getYRotationMatrix(30 * iter++, 0),   //
//											 getXRotationMatrix(-25, 0),          //
//											 getTranslationMatrix(0, .1f, 2.1f)};
//	renderer->camera->cameraToWorld = getTransformationMatrix(trans, 4);

//	rendererInit(renderer);
//	rayTrace(renderer, screen);
//	printf("%d \n", iter);
//}

void engineRun(RenderArgs *args) {
	printf("Starting render engine...\n");

	const int WIDTH = (int)args->resolution.x, HEIGHT = (int)args->resolution.y;

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

	const char *path = "input/torus.obj";
	if (*args->objPath) {
		path = args->objPath;
	}

	double scale = 1;
	parseObj(path, &t, &size, &scale, args->baseTriangle);
	scale /= args->scale;

	Camera camera;
	Matrix4x4 trans[] = {
			getScaleMatrix(scale, scale, scale),           //
			getXRotationMatrix(args->modelRotation.x, 0),  // model orientation
			getYRotationMatrix(args->modelRotation.y, 0),  //
			getTranslationMatrix(0, 0, 2.5),               // camera position
			getXRotationMatrix(args->cameraRotation.x, 0), // camera orientation
			getYRotationMatrix(args->cameraRotation.y, 0), //
			getZRotationMatrix(args->cameraRotation.z, 0), //
	};
	Matrix4x4 camToWorld = getTransformationMatrix(trans, 7);

	camera.width = WIDTH;
	camera.height = HEIGHT;
	camera.fov = args->fov;
	camera.cameraToWorld = camToWorld;

	Scene scene;
	scene.bkgR = args->bkgColor.x; // .4f
	scene.bkgG = args->bkgColor.y;
	scene.bkgB = args->bkgColor.z;
	scene.ambientLight = args->ambientLight;           // .15
	scene.kSpecularExponent = args->kSpecularExponent; // 3

	scene.triangles = t;
	scene.nTriangles = size;

	//	PointLight pointLights[1];
	//	Vector3D pointLightLoc1 = {0, -40, 50};
	//	pointLights[0].point = &pointLightLoc1;
	//	pointLights[0].intensity = 20;
	//	Vector3D pointLightCoeffs1 = {2, .01, .005};
	//	pointLights[0].attenuationCoeffs = &pointLightCoeffs1;
	//	scene.pointLights = pointLights;

	DirectionalLight dirLights[4];
	Vector3D dirLightDir0 = {1, -.5, -1};
	dirLights[0].direction = norm(&dirLightDir0, &dirLightDir0);
	dirLights[0].intensity = .5;
	Vector3D dirLightDir1 = {1, -.5, 1};
	dirLights[1].direction = norm(&dirLightDir1, &dirLightDir1);
	dirLights[1].intensity = .5;
	Vector3D dirLightDir2 = {-1, -.5, -1};
	dirLights[2].direction = norm(&dirLightDir2, &dirLightDir2);
	dirLights[2].intensity = .5;
	scene.directionalLights = dirLights;
	Vector3D dirLightDir3 = {-1, -.5, 1};
	dirLights[3].direction = norm(&dirLightDir3, &dirLightDir3);
	dirLights[3].intensity = .5;
	scene.directionalLights = dirLights;

	//	SpotLight spotLights[4];
	//	Vector3D spotLightLoc0 = {-60, -20, 100};
	//	Vector3D spotLightDir0 = {1, -.25f, 0};
	//	Vector3D spotLightCoeffs0 = {1, 1, 3.0f};
	//	spotLights[0].point = &spotLightLoc0;
	//	spotLights[0].direction = norm(&spotLightDir0, &spotLightDir0);
	//	spotLights[0].attenuationCoeffs = &spotLightCoeffs0;
	//	spotLights[0].intensity = 2000.0f;

	// *** light counts
	scene.nPointLights = 0;
	scene.nDirectionalLights = 4;
	scene.nSpotLights = 0;

	Renderer renderer = {&camera, &scene, 0, 0, 0};
	renderer.nThreads = args->nThreads;                         // 96
	renderer.nTraces = args->nTraces;                           // 1
	renderer.nAntialiasingSamples = args->nAntialiasingSamples; // 3

	printf("Initializing visualizer...\n");

	Visualizer vis;
	int loadVideo = args->loadVideo; // VIDEO DOES NOT CURRENTLY WORK

	if (loadVideo) {
		printf("Video unsupported at this moment \n");
		//		visInitVideo(&vis, WIDTH, HEIGHT, &render, &renderer);
		//		visShowVideo(&vis);
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

		printf("Saving...\n");
		const char *out = "output.tga";
		if (*args->outputPath) {
			out = args->outputPath;
		}
		printf("%d \n", saveToTGA(out, vis.pixels, (short)WIDTH, (short)HEIGHT));

		printf("Showing...\n");
		visShowStill(&vis);
	}

	free(t);
}
