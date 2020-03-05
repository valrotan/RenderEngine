#pragma once

// Intended to parse obj files to work with this Rendering Engine

#include "../math/renderMath.h"
#include "stdio.h"
#include "stdlib.h"

void parseObj(char *path, Triangle3D **trigList, int *size, float *scale);
char *parser_strtok_r(char *str, const char *delim, int newstr);
void centerVertecies(Vector3D* verts);