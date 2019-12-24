#include "renderer.h"

char *rayCast(Camera *camera, Scene *scene, int width, int height);

Ray3D constructRayThroughPixel(Camera *camera, int x, int y);

void traceRay(Scene *scene, Ray3D *ray, char **currentPixel);

void findIntersection(Scene *scene, Ray3D *ray);

void getColor(); // TBD based on traceRay implementation
