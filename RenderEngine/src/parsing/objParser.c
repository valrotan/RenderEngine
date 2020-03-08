#include "objParser.h"

void parseObj(char *path, Triangle3D **trigList, int *size, float *scale) {
	FILE *fpIn = openFile(path);

	char *materialsPath[100]; // for future material path parsing

	char line[1000]; // buffer line

	int sizeVerts = 1000; // the current size of vertex array
	Vector3D *verts = (Vector3D *)malloc(sizeVerts * sizeof(Vector3D));
	int vertCount = 0;

	int sizeFaces = 1000;
	StackNode* facesStack = (StackNode*)malloc(sizeof(StackNode));
	Vector3D* centerElement = (Vector3D*)malloc(sizeof(Vector3D)); // used for centering vertecies
	centerElement->x = 0;
	centerElement->y = 0;
	centerElement->z = 0;

	int facesCount = 0;

	while (fscanf(fpIn, "%s", line) != EOF) {
		
		//skip comment lines in the file
		if (strstr(line, "#")) {
			fscanf(fpIn, "%[^\n]s", line);
		}

		//capture path to the mtl file
		if (strstr(line, "mtllib")) {
			fscanf(fpIn, "%s", line);
			strcpy(materialsPath, line);
		}

		//capturing data lines
		if (strlen(line) == 1) {

			//skipping object line
			if (strstr(line, "o")) {
				fscanf(fpIn, "%s", line);

			// capturing vertecies
			} else if (line[0] == 'v' &&
								 line[1] == '\0') {
				//checking if need to expand the size of vertecies if reached max
				if (vertCount >= sizeVerts) { 
					sizeVerts *= 2;
					Vector3D *temp = realloc(verts, sizeVerts * sizeof(Vector3D));
					if (!temp) {
						printf("Error, wasn't able to reallocate verts!\n");
					}
					verts = temp;
				}

				float x, y, z;
				fscanf(fpIn, "%f %f %f", &x, &y, &z);
				Vector3D a = {x, y, z};
				add(centerElement, &a, centerElement);
				verts[vertCount] = a;
				vertCount++;
			
			// parse faces
			} else if (line[0] == 'f' && line[1] == '\0') {
				fscanf(fpIn, "%[^\n]s", line);

				int *indexes, vertNum;

				// parsing the face line to create multiple triangles if more
				// than 3 vertecies
				parseFaceLine(line, &vertNum, &indexes);

				int first = indexes[0];
				for (int i = 1; i + 1 < vertNum; i++) {
					Vector3D *aa = verts + first - 1;
					Vector3D *bb = verts + indexes[i] - 1;
					Vector3D *cc = verts + indexes[i + 1] - 1;
					Triangle3D trig = {
							aa,
							bb,
							cc,
							0,
							0,
							0,
							0.25f,
							.25f,
							.1f};

					Triangle3D* pTemp = (Triangle3D*)malloc(sizeof(Triangle3D));
					*pTemp = trig;
					facesStack = push(facesStack, pTemp);
					facesCount++;
				}
				free(indexes);
			}
		}
	}
	Triangle3D* triggs = (Triangle3D*)malloc(facesCount * sizeof(Triangle3D));
	int i = 0;

	divide(centerElement, vertCount, centerElement); // calculate average x,y,z

	float xMax = 0, xMin = 0;
	float yMax = 0, yMin = 0;
	float zMax = 0, zMin = 0;


	for (int i = 0; i < vertCount; i++) {
		sub(&verts[i], centerElement, &verts[i]);
		setMinMax(&(verts[i].x), &xMin, &xMax);
		setMinMax(&(verts[i].y), &yMin, &yMax);
		setMinMax(&(verts[i].z), &zMin, &zMax);
	}


	xMax = (xMax - xMin) / 1.4;
	yMax = yMax - yMin;
	zMax = (zMax - zMin) / 1.4;
	
	*scale = xMax > yMax ? xMax : yMax;
	*scale = *scale > zMax ? *scale : zMax;
	*scale /= 2;

	while (facesStack && i < facesCount) {
		Triangle3D* n = facesStack->data;
		Vector3D centroid;
		divide(add(add(n->p1, n->p2, &centroid), n->p3, &centroid), 2*(*scale), &centroid);
		n->colorR = sinf(centroid.x)/2 + .5f;
		n->colorG = sinf(centroid.y) / 2 + .5f;
		n->colorB = sinf(centroid.z) / 2 + .5f;
		// funky soft colors
		// n->colorR = .5f + sinf(cosf(centroid.x) + centroid.y) / 1;
		// n->colorG = .5f + sinf(cosf(centroid.z) + centroid.x) / 1; 
		// n->colorB = .5f + sinf(cosf(centroid.y) + centroid.z) / 1;
		pop(&facesStack);
		triggs[i] = *n;
		i++;
	}

	*trigList = triggs;
	*size = facesCount;

	printf("\nFaces list size: %d\n", sizeFaces);
	printf("Size of the triangle: %ud\n", sizeof(Triangle3D));
	printf("Memory wasted: %ud bytes\n", (sizeFaces - facesCount < 0 ? 0 : (sizeFaces - facesCount) * sizeof(Triangle3D)) );
	printf("Number of verts: %d\n", vertCount);
	printf("Number of faces: %d\n\n", facesCount);

	line[0] = '\0';

	free(centerElement);
	free(facesStack);

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
