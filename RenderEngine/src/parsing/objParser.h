#pragma once

// Intended to parse obj files to work with this Rendering Engine

#include "../math/renderMath.h"
#include "stack.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// helper functions

// Open the path for the given file
FILE *openFile(const char *path);

// Parsing a face line to capture the vertecies of an object
// Pre:		str - line string
//		vertN  number of vertecies obtained
//		verts - the vertecy indexes taht got captured
// Post: capture and set the proper arguments
// Returns: void
void parseFaceLine(char *str, int *vertN, int **verts);

/*
	Parses an obj file and outputs triangles that is interpretable by the program
	PRE:	path - path to the .obj file
			trigList - a pointer to the triangle list
			size - output int for the number of triangles
			scale - the scale factor to scale all of the models to be of similar size
	POST:
			File is parsed and all of the pre variables are set properly
	returns: void
*/
void parseObj(const char *path, Triangle3D **trigList, int *size, double *scale,
							Triangle3D baseTriangle);
