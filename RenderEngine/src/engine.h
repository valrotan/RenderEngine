#include "renderer/renderer.h"

typedef struct {
	Vector2D resolution;
	const char *objPath;
	const char *outputPath;

	float fov;
	float scale;

	Vector3D bkgColor;
	float ambientLight;
	int kSpecularExponent;

	int nAntialiasingSamples;
	int nTraces;
	int nThreads;

	int loadVideo;
} RenderArgs;

RenderArgs engineDefaultArgs(void);

void engineRun(RenderArgs *args);
