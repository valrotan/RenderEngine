#define _CRT_SECURE_NO_WARNINGS
#include "objParser.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

char *trimwhitespace(char *str);
FILE *openFile(char *path);
void parseFaceLine(char *str, int *vertN, int **verts);
/*
	Similar to strtok, takes in string and delim, pass the same string to extract
	all of the tokens does not change the string PRE:	str - string to tokenize
			delim - a set of character delimeters
			newstr - bool val if want to parse the next string
	POST:
		token is extracted, currIndex is changed
	returns: extracted token
*/

void parseObj(char *path, Triangle3D **trigList, int *size) {
	FILE *fpIn = openFile(path);
	char *materialsPath[100];

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
				// printf("LINE: %s\n", line);
			} else if (line[0] == 'v' &&
								 line[1] == '\0') { // strstr(line, "v") && !strstr(line,"vn")
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
				// printf("GOT: %f %f %f\n", x, y, z);
				Vector3D a = {x, y, z};
				verts[vertCount] = a;
				vertCount++;
			} else if (line[0] == 'f' && line[1] == '\0') {
				if (facesCount + 10 >= sizeFaces) {
					sizeFaces *= 2;
					tempFaces = realloc(faces, sizeFaces * sizeof(Triangle3D));
					if (!tempFaces) {
						printf("Error with allocation!\n");
					}
					faces = tempFaces;
				}
				fscanf(fpIn, "%[^\n]s", line);
				// printf("%s\n", line);
				int *indexes, vertNum;
				parseFaceLine(line, &vertNum, &indexes);
				int first = indexes[0];
				for (int i = 1; i + 1 < vertNum; i++) {
					Vector3D *aa = verts + first - 1;
					Vector3D *bb = verts + indexes[i] - 1;
					Vector3D *cc = verts + indexes[i + 1] - 1;
					Vector3D centroid;
					divide(add(add(aa, bb, &centroid), cc, &centroid), 3, &centroid);
					// printf("%3d| VERTS: %d %d %d\n", facesCount, indexes[i], indexes[i
					// + 1], indexes[i + 2]);
					Triangle3D trig = {
							aa,
							bb,
							cc,
							0,
							.5f + sinf(cosf(centroid.x) + centroid.y) / 2,
							.5f + sinf(cosf(centroid.y) + centroid.z) / 2,
							.5f + sinf(cosf(centroid.z) + centroid.x) / 2,
							0.25f,
							.25f,
							.1f};
					faces[facesCount] = trig;
					facesCount++;
				}
				free(indexes);
			}
		}
	}

	// printf("VERTC: %d\n", vertCount);
	// printf("FACEC: %d\n", facesCount);
	*trigList = faces;
	*size = facesCount;
	printf("Number of verts: %d\n", vertCount);
	printf("Number of faces: %d\n", facesCount);

	line[0] = '\0';
	// printf("TRIG TEST: (%f, %f, %f)(%f, %f, %f)(%f, %f, %f)\n", faces[0].p1->x,
	//			 faces[0].p1->y, faces[0].p1->z, faces[0].p2->x, faces[0].p2->y,
	//			 faces[0].p2->z, faces[0].p3->x, faces[0].p3->y, faces[0].p3->z);
	if (fclose(fpIn) != 0) {
		exit(3);
	}
}

void parseFaceLine(char *line, int *vertNumber, int **v) {
	int spaces = 0;
	for (int i = 0; line[i] != '\0'; i++) {
		if (line[i] == ' ') {
			spaces++;
		}
	}
	line += 1;
	int *verts = (int *)malloc(spaces * sizeof(int));

	for (int i = 0; i < spaces; i++) {
		char temp[100];
		sscanf(line, "%[^\040\t]s", temp);

		if (i < spaces - 1) {
			line = strpbrk(line, "\t\040");
			line = strpbrk(line, "0123456789");
		}

		if (strstr(temp, "/")) {
			// char* aStr = parser_strtok_r(line, "/", 1);
			// fscanf(fpIn, "%s", line);
			// char* bStr = parser_strtok_r(line, "/", 1);
			// fscanf(fpIn, "%s", line);
			// char* cStr = parser_strtok_r(line, "/", 1);
			// sscanf(aStr, "%d", &a);
			// sscanf(bStr, "%d", &b);
			// sscanf(cStr, "%d", &c);
			sscanf(temp, "%d", verts + i);
		} else {
			sscanf(temp, "%d", verts + i);
		}
	}
	*vertNumber = spaces;
	*v = verts;
}

FILE *openFile(char *path) {
	FILE *fpIn;
	if (!(fpIn = fopen(path, "r"))) {
		printf("File was not found!\n");
		exit(2);
	}
	return fpIn;
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

char *parser_strtok_r(char *str, const char *delim, int newstr) {
	static int currIndex = 0;
	if (newstr)
		currIndex = 0;
	if (!str || !delim || str[currIndex] == '\0')
		return NULL;

	char *pS = str;
	char *W = (char *)malloc(100 * sizeof(char));
	int i = currIndex, k = 0, j;
	int br = 0; // bool
	while (str[i] != '\0' && !br) {
		j = 0;
		while (delim[j]) {
			if (str[i] != delim[j]) {
				W[k] = str[i];
				j++;
				k++;
			} else {
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
