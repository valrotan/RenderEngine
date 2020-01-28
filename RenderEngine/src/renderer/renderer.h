#pragma once

#include "../math/renderMath.h"

// todo: change screenZ to fov
typedef struct {
	Matrix4x4 cameraToWorld;
	float fov;
} Camera;

typedef struct {
	float intensity;
	Vector3D *point;
	Vector3D *attenuationCoeffs;
} PointLight;

typedef struct {
	float intensity;
	Vector3D *direction;
} DirectionalLight;

typedef struct {
	float intensity;
	Vector3D *point;
	Vector3D *direction;
	Vector3D *attenuationCoeffs;
} SpotLight;

// Only triangles for now I guess
typedef struct {
	float ambientLight;
	unsigned char bkgR, bkgG, bkgB;
	float kSpecularExponent;
	Triangle3D *triangles;
	int nTriangles;
	PointLight *pointLights;
	int nPointLights;
	DirectionalLight *directionalLights;
	int nDirectionalLights;
	SpotLight *spotLights;
	int nSpotLights;
} Scene;

typedef struct {
	Camera *camera;
	Scene *scene;
	int nThreads;
} Renderer;

void rendererInit(Renderer *renderer);

void rayTrace(Renderer *renderer, unsigned char *screen, int width, int height);

Ray3D *constructRayThroughPixel(Camera *camera, int x, int y, int width,
																int height);

unsigned char *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int depth);

Intersection3D *findIntersection(Scene *scene, Ray3D *ray);

unsigned char *getColor(Camera *camera, Scene *scene,
												Intersection3D *intersection, int depth);

void calcPointLights(Scene *scene, Intersection3D *intersection,
										 Vector3D *normal, Vector3D *view, float *id, float *is);

void calcDirectionalLights(Scene *scene, Intersection3D *intersection,
													 Vector3D *normal, Vector3D *view, float *id,
													 float *is);

void calcSpotLights(Scene *scene, Intersection3D *intersection,
										Vector3D *normal, Vector3D *view, float *id, float *is);
