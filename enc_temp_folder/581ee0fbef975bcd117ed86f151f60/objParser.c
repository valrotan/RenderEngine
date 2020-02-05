#define _CRT_SECURE_NO_WARNINGS
#include "objParser.h"
#include <stdio.h>

char *trimwhitespace(char *str);

void parseObj(char *path, Triangle3D **trigList, int *size) {
	FILE *fpIn;
	char *materialsPath[100];

	if (!(fpIn = fopen(path, "r"))) {
		printf("File was not found!\n");
		exit(2);
	}
	char line[1000];
	// char* pos = line;

	int sizeVerts = 10000;
	Vector3D *verts = (Vector3D *)malloc(sizeVerts * sizeof(Vector3D));
	int vertCount = 0;

	int sizeFaces = 10000;
	Triangle3D *faces = (Triangle3D *)malloc(sizeFaces * sizeof(Triangle3D));
	Triangle3D *tempFaces;
	int facesCount = 0;

	while (fscanf(fpIn, "%s", line) != EOF) {
		// printf("\'%s\'\n",line);
		if (strstr(line, "#")) {
			fscanf(fpIn, "%[^\n]s", line);
		}
		if (strstr(line, "mtllib")) {
			fscanf(fpIn, "%s", line);
			strcpy(materialsPath, line);
		}
		if (strlen(line) == 1) {
			if (strstr(line, "o")) {
				fscanf(fpIn, "%s", line);
				printf("LINE: %s\n", line);
			} else if (strstr(line, "v")) {
				if (vertCount >= sizeVerts) {
					sizeVerts += 500;
					verts = realloc(verts, sizeVerts);
				}
				float x, y, z;
				fscanf(fpIn, "%f %f %f", &x, &y, &z);
				Vector3D a = {x, y, z};
				verts[vertCount] = a;
				vertCount++;
			} else if (strstr(line, "f")) {
				if (facesCount >= sizeFaces) {
					sizeFaces += 500;
					printf("Realloocing + %d", sizeFaces);
					tempFaces = realloc(faces, sizeFaces);
					faces = tempFaces;
				}
				int a, b, c;
				fscanf(fpIn, "%d %d %d", &a, &b, &c);
				Vector3D *aa = verts + a - 1;
				Vector3D *bb = verts + b - 1;
				Vector3D *cc = verts + c - 1;

				// printf("%d| VERTS: %d %d %d\n", facesCount, a, b, c);

<<<<<<< HEAD
				Triangle3D trig = {aa, bb, cc, 0, 0 / 255.0f,
												  0 / 255.0f, 225 / 255.0f, 0.25f, .25f, .1f};
=======
				Triangle3D trig = {aa, bb, cc, 0, 200 / 255.0f,
													20 / 255.0f, 60 / 255.0f, 0.25f, .5f, 0.1f};
>>>>>>> 3dc8a9c06d5b4af646aa3357155d4a9a84f12362
				faces[facesCount] = trig;
				facesCount++;
			}
		}
	}

	//printf("VERTC: %d\n", vertCount);
	//printf("FACEC: %d\n", facesCount);
	*trigList = faces;
	*size = facesCount;

	line[0] = '\0';
	//printf("TRIG TEST: (%f, %f, %f)(%f, %f, %f)(%f, %f, %f)\n", faces[0].p1->x,
	//			 faces[0].p1->y, faces[0].p1->z, faces[0].p2->x, faces[0].p2->y,
	//			 faces[0].p2->z, faces[0].p3->x, faces[0].p3->y, faces[0].p3->z);
	if (fclose(fpIn) != 0) {
		exit(3);
	}
}

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str) {
	char *end;

	// Trim leading space
	while (isspace((unsigned char)*str))
		str++;

	if (*str == 0) // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end))
		end--;

	// Write new null terminator character
	end[1] = '\0';

	return str;
}
