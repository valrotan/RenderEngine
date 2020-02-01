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

typedef struct BoundingVolume {
	struct BoundingVolume *children;
	int nChildren;
	Triangle3D **triangles;
	int nTriangles;
	Vector3D low, high;
} BoundingVolume;

// Only triangles for now I guess
typedef struct {
	float ambientLight;
	float bkgR, bkgG, bkgB;
	float kSpecularExponent;
	Triangle3D *triangles;
	int nTriangles;
	PointLight *pointLights;
	int nPointLights;
	DirectionalLight *directionalLights;
	int nDirectionalLights;
	SpotLight *spotLights;
	int nSpotLights;
	BoundingVolume *bv;
} Scene;

typedef struct {
	Camera *camera;
	Scene *scene;
	int nThreads;
} Renderer;

typedef struct {
	Renderer *renderer;
	unsigned char *screen;
	int width;
	int height;
	int startLine;
	int stopLine;
} RendererSegment;

void rendererInit(Renderer *renderer);

void rayTrace(Renderer *renderer, unsigned char *screen, int width, int height);

Ray3D *constructRayThroughPixel(Camera *camera, int x, int y, int width,
																int height, Ray3D *ray);

float *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int depth,
								float *rgb);

Intersection3D *findIntersection(Scene *scene, Ray3D *ray);

float *getColor(Camera *camera, Scene *scene, Intersection3D *intersection,
								int depth, float *rgb);

void calcPointLights(Scene *scene, Intersection3D *intersection,
										 Vector3D *normal, Vector3D *view, float *id, float *is);

void calcDirectionalLights(Scene *scene, Intersection3D *intersection,
													 Vector3D *normal, Vector3D *view, float *id,
													 float *is);

void calcSpotLights(Scene *scene, Intersection3D *intersection,
										Vector3D *normal, Vector3D *view, float *id, float *is);

BoundingVolume *constructBoundingVolumes(BoundingVolume *bv);

int smitsBoxIntersect(BoundingVolume *bv, Ray3D *ray);

Intersection3D *findIntersectionBV(BoundingVolume *bv, Ray3D *ray,
																	 Intersection3D *inter);
