#include "renderer/renderer.h"

typedef struct {
	Vector2D resolution;
	const char *objPath;
	const char *outputPath;

	double fov;
	double scale;

	Vector3D bkgColor;
	double ambientLight;
	int kSpecularExponent;

	int nAntialiasingSamples;
	int nTraces;
	int nThreads;

	int loadVideo;

	Triangle3D baseTriangle;
} RenderArgs;

RenderArgs engineDefaultArgs(void);

void engineRun(RenderArgs *args);
