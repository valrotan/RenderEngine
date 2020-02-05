#define _CRT_SECURE_NO_WARNINGS
#include "objParser.h"
#include <stdio.h>

char *trimwhitespace(char *str);

/*
	Similar to strtok, takes in string and delim, pass the same string to extract all of the tokens
	does not change the string
	PRE:	str - string to tokenize
			delim - a set of character delimeters
			newstr - bool val if want to parse the next string
	POST:
		token is extracted, currIndex is changed
	returns: extracted token
*/
char* strtok_r(char* str, const char* delim, int newstr);

void parseObj(char *path, Triangle3D **trigList, int *size) {
	FILE *fpIn;
	char *materialsPath[100];

	if (!(fpIn = fopen(path, "r"))) {
		printf("File was not found!\n");
		exit(2);
	}
	char line[1000];
	// char* pos = line;

	int sizeVerts = 1000;
	Vector3D *verts = (Vector3D *)malloc(sizeVerts * sizeof(Vector3D));
	int vertCount = 0;

	int sizeFaces = 1000;
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
				//printf("LINE: %s\n", line);
			} else if (strstr(line, "v")) {
				if (vertCount >= sizeVerts) {
					sizeVerts *= 2;
					Vector3D *temp = realloc(verts, sizeVerts * sizeof(Vector3D));
					if (!temp) {
						printf("SOME ERROR\n");
					}
					verts = temp;
				}
				float x, y, z;
				fscanf(fpIn, "%f %f %f", &x, &y, &z); 
				Vector3D a = {x, y, z};
				verts[vertCount] = a;
				vertCount++;
			} else if (strstr(line, "f")) {
				if (facesCount >= sizeFaces) {
					sizeFaces *= 2;
					tempFaces = realloc(faces, sizeFaces * sizeof(Triangle3D));
					if (!tempFaces) {
						printf("Error with allocation!\n");
					}
					faces = tempFaces;
				}
				fscanf(fpIn, "%s", line);
				int a, b, c;
				if (strstr(line, "/")) {
					char* aStr = strtok_r(line, "/", 1);
					fscanf(fpIn, "%s", line);
					char* bStr = strtok_r(line, "/", 1);
					fscanf(fpIn, "%s", line);
					char* cStr = strtok_r(line, "/", 1);
					sscanf(aStr, "%d", &a);
					sscanf(bStr, "%d", &b);
					sscanf(cStr, "%d", &c);
				}
				else {
					sscanf(line, "%d", &a);
				}
				
				fscanf(fpIn, "%d %d", &b, &c);
				Vector3D *aa = verts + a - 1;
				Vector3D *bb = verts + b - 1;
				Vector3D *cc = verts + c - 1;

				// printf("%d| VERTS: %d %d %d\n", facesCount, a, b, c);

				Triangle3D trig = {aa, bb, cc, 0, 0 / 255.0f,
												  0 / 255.0f, 225 / 255.0f, 0.25f, .25f, .1f};
				faces[facesCount] = trig;
				facesCount++;
				fscanf(fpIn, "%[^\n]s", line); // to skip all of the other points if there's more than 3
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


char* strtok_r(char* str, const char* delim, int newstr) {
	static int currIndex = 0;
	if (newstr)
		currIndex = 0;
	if (!str || !delim || str[currIndex] == '\0')
		return NULL;

	char* pS = str;
	char* W = (char*)malloc(100 * sizeof(char));
	int i = currIndex, k = 0, j;
	int br = 0; // bool
	while (str[i] != '\0' && !br) {
		j = 0;
		while (delim[j]) {
			if (str[i] != delim[j]) {
				W[k] = str[i];
				j++;
				k++;
			}
			else {
				br = 1;
				break;
			}
		}
		i++;
	}
	W[k] = '\0';
	currIndex = i;
	return W;
}