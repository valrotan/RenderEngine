#pragma once

typedef struct {} Camera;
typedef struct {} Scene;


char* rayCast(Camera camera, Scene scene, int width, int height);

void ConstructRayThroughPixel();

void FindIntersection();

void GetColor();
