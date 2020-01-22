#include "renderer/renderer.h"
#include "visualizer/visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

void rayTraceDemo() {
	printf("Starting render engine...\n");

	int WIDTH = 512, HEIGHT = 512;

	printf("Initializing visualizer...\n");
	visInit(WIDTH, HEIGHT);
	unsigned char *screen = visGetPixbuf();

	//printf("Initializing renderer...\n");
	//Camera camera;
	//Vector3D pos = {0, 0, 200};
	//Vector3D dir = {0, 0, -1};
	//Vector3D up = {0, -1, 0};
	//camera.pos = pos;
	//camera.dir = dir;
	//camera.up = up;
	//camera.screenZ = 450;

	printf("Initializing renderer...\n");
	Camera camera;
//	Vector3D pos = { 0, 0, 200 };
//	Vector3D dir = { 0, 0, -1 };
//	Vector3D up = { 0, -1, 0 };
	Matrix4x4 trans[] = {getTranslationMatrix(0, 0, 200), getZRotationMatrix(0,0)};
	Matrix4x4 camToWorld = getTransformationMatrix(trans, 2);
	
//	printf("Orig pos: (%f, %f, %f)\n", pos.x,pos.y,pos.z);

//	camera.pos = applyTransformation(pos,camToWorld);
//	camera.dir = applyTransformation(dir,camToWorld);
//	camera.up = applyTransformation(up,camToWorld);
//	camera.pos = pos;
//	camera.dir = dir;
//	camera.up = up;
	camera.screenZ = 60;
	camera.cameraToWorld = camToWorld;

//	Triangle3D *t = (Triangle3D *) malloc(sizeof (Triangle3D) * 8);
	Triangle3D t[8];
//	printf("%d \n", sizeof(t));

	// test triangles
//	Vector3D a = {0, 50, 500}; // behind camera
//	Vector3D b = {50, 100, 500};
//	Vector3D c = {50, 50, 500};
//	Vector3D d = {0, 50, 0}; // same y < 0
//	Vector3D e = {20, 50, 0};
//	Vector3D f = {50, 50, -50};
//	Triangle3D temp1 = {&a, &b, &c, 0, 255, 0, 0, .1f, .25f, .25f};
//	Triangle3D temp2 = {&d, &e, &f, 0, 255, 0, 0, .1f, .25f, .25f};
//	Vector3D ab = {0, 0, 0}; // same y = 0 : should be invisible
//	Vector3D bb = {-20, 0, -50};
//	Vector3D cb = {-50, 0, -50};
//	Vector3D db = {0, 50, 0}; // normal
//	Vector3D eb = {-50, 100, -50};
//	Vector3D fb = {0, 100, 0};
//	Triangle3D temp3 = {&ab, &bb, &cb, 0, 0, 255, 0, 0, .25f, .25f};
//	Triangle3D temp4 = {&db, &eb, &fb, 0, 0, 255, 0, 0, .25f, .25f};
//	Vector3D ac = {0, 50, 0}; // same y < 0
//	Vector3D bc = {0, 50, -100};
//	Vector3D cc = {-50, 50, -50};
//	Vector3D dc = {0, 50, 0}; // almost same y < 0
//	Vector3D ec = {50, 50, -50};
//	Vector3D fc = {0, 50.1f, -100};
//	Triangle3D temp5 = {&ac, &bc, &cc, 0, 0, 0, 255, .75f, .75f, .25f};
//	Triangle3D temp6 = {&dc, &ec, &fc, 0, 0, 0, 255, .75f, .75f, .25f};
//	Vector3D ad = {0, 0, 0}; // normal
//	Vector3D bd = {50, 30, 60};
//	Vector3D cd = {50, -30, 60};
//	Vector3D dd = {0, 0, 0}; // normal
//	Vector3D ed = {-50, -50, 40};
//	Vector3D fd = {-50, 50, 40};
//	Triangle3D temp7 = {&ad, &bd, &cd, 0, 0, 0, 0, .0f, 1.0f, .05f};
//	Triangle3D temp8 = {&dd, &ed, &fd, 0, 0, 0, 255, .0f, .5f, .25f};

	// cool triangles
	Vector3D a = {0, -50, 0};
	Vector3D b = {50, -50, -50};
	Vector3D c = {50, -100, -50};
	Vector3D d = {0, -50, 0};
	Vector3D e = {0, -100, 0};
	Vector3D f = {50, -100, -50};
	Triangle3D temp1 = {&a, &b, &c, 0, 200, 0, 0, 0, .25f, .25f};
	Triangle3D temp2 = {&d, &e, &f, 0, 200, 0, 0, 0, .25f, .25f};
	Vector3D ab = {0, -50, 0};
	Vector3D bb = {-50, -50, -50};
	Vector3D cb = {-50, -100, -50};
	Vector3D db = {0, -50, 0};
	Vector3D eb = {-50, -100, -50};
	Vector3D fb = {0, -100, 0};
	Triangle3D temp3 = {&ab, &bb, &cb, 0, 50, 200, 50, 0, .25f, .25f};
	Triangle3D temp4 = {&db, &eb, &fb, 0, 50, 200, 50, 0, .25f, .25f};
	Vector3D ac = {0, -50, 0};
	Vector3D bc = {-50, -50, -50};
	Vector3D cc = {0, -50, -100};
	Vector3D dc = {0, -50, 0};
	Vector3D ec = {50, -50, -50};
	Vector3D fc = {0, -50, -100};
	Triangle3D temp5 = {&ac, &bc, &cc, 0, 50, 50, 200, 0, .25f, .25f};
	Triangle3D temp6 = {&dc, &ec, &fc, 0, 50, 50, 200, 0, .25f, .25f};
	Vector3D ad = {0, -10, 10};
	Vector3D bd = {50, -60, -20};
	Vector3D cd = {50, -10, -20};
	Vector3D dd = {0, -10, 10};
	Vector3D ed = {-50, -10, -20};
	Vector3D fd = {-50, -60, -20};
	Triangle3D temp7 = {&ad, &bd, &cd, 0, 80, 80, 80, 0.25, .25f, .1f};
	Triangle3D temp8 = {&dd, &ed, &fd, 0, 50, 50, 200, 0, .25f, .25f};

	t[0] = temp1;
	t[1] = temp2;
	t[2] = temp3;
	t[3] = temp4;
	t[4] = temp5;
	t[5] = temp6;
	t[6] = temp7;
	t[7] = temp8;

	Scene scene;
	scene.bkgR = 100;
	scene.bkgG = 100;
	scene.bkgB = 100;
	scene.ambientLight = .45f;

	scene.triangles = &t[0];
	scene.nTriangles = 8;

	PointLight pointLights[4];
	Vector3D pointLightLoc1 = {0, -40, -50};
	pointLights[0].point = &pointLightLoc1;
	pointLights[0].intensity = 10;
	Vector3D pointLightCoeffs1 = {2, .01f, .005f};
	pointLights[0].attenuationCoeffs = &pointLightCoeffs1;

	scene.pointLights = &pointLights[0];

	DirectionalLight dirLights[4];
	Vector3D dirLightDir0 = {0, -1, -1};
	dirLights[0].direction = norm(&dirLightDir0, &dirLightDir0);
	dirLights[0].intensity = .5f;

	scene.directionalLights = dirLights;

	SpotLight spotLights[4];
	Vector3D spotLightLoc0 = {-20, -20, 50};
	Vector3D spotLightDir0 = {1, -.25f, 0};
	Vector3D spotLightCoeffs0 = {1, 0, 6.0f};
	spotLights[0].point = &spotLightLoc0;
	spotLights[0].direction = norm(&spotLightDir0, &spotLightDir0);
	spotLights[0].attenuationCoeffs = &spotLightCoeffs0;
	spotLights[0].intensity =	1000.0f;

	Vector3D spotLightLoc1 = {20, -20, 50};
	Vector3D spotLightDir1 = {-1, -.25f, 0};
	Vector3D spotLightCoeffs1 = {1, 0, 6.0f};
	spotLights[1].point = &spotLightLoc1;
	spotLights[1].direction = norm(&spotLightDir1, &spotLightDir1);
	spotLights[1].attenuationCoeffs = &spotLightCoeffs1;
	spotLights[1].intensity =	1000.0f;

	scene.spotLights = spotLights;

	// *** light counts
	scene.nPointLights = 1;
	scene.nDirectionalLights = 1;
	scene.nSpotLights = 2;

	Renderer renderer = {&camera, &scene};

	rendererInit(&renderer);

	printf("Raycasting...\n");

	
	struct timeb start, end;
	int diff;
	ftime(&start);

	rayTrace(&camera, &scene, screen, WIDTH, HEIGHT);
	ftime(&end);
	diff = (int)(1000.0 * (end.time - start.time)
		+ (end.millitm - start.millitm));

	printf("Render took %u milliseconds \n", diff);

	printf("Showing...\n");

	visShowStill();
}

int main() {
	rayTraceDemo();
	return 0;
}
