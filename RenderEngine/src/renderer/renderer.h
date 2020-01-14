#pragma once

#include "../math/renderMath.h"

// todo: change screenZ to fov
typedef struct {
  Vector3D pos;
  Vector3D dir;
  Vector3D up;
  //	float xfov;
  //	float yfov;

  // distance of screen from camera point
  // makes ray through pixel calculation easier
  // for 256x256 150 is a good value
  int screenZ;
} Camera;

// sample camera:
// (150,0,0)
// (-150,0,0)
// (0,0,1)
// 150

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
  Triangle3D *triangles;
  int nTriangles;
  PointLight *pointLights;
  int nPointLights;
} Scene;

typedef struct {
  Camera *camera;
  Scene *scene;
} Renderer;

void rendererInit(Renderer *renderer);

void rayTrace(Camera *camera, Scene *scene, unsigned char *screen, int width,
			  int height);

Ray3D *constructRayThroughPixel(Camera *camera, int x, int y);

unsigned char *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int curDepth,
						int maxDepth);

Intersection3D *findIntersection(Scene *scene, Ray3D *ray);

unsigned char *getColor(Camera *camera, Scene *scene,
						Intersection3D *intersection, int curDepth,
						int maxDepth);
