#include "objParser.h"

void parseObj(const char *path, Triangle3D **trigList, int *size, double *scale,
							Triangle3D baseTriangle) {
	FILE *fpIn = openFile(path);

	char *materialsPath[100]; // for future material path parsing

	char line[1000]; // buffer line

	int sizeVerts = 1000; // the current size of vertex array
	Vector3D *verts = (Vector3D *)malloc(sizeVerts * sizeof(Vector3D));
	int vertCount = 0;

	int sizeFaces = 1000;
	StackNode *facesStack = (StackNode *)malloc(sizeof(StackNode));
	Vector3D *centerElement =
			(Vector3D *)malloc(sizeof(Vector3D)); // used for centering vertecies
	centerElement->x = 0;
	centerElement->y = 0;
	centerElement->z = 0;

	int facesCount = 0;

	while (fscanf(fpIn, "%s", line) != EOF) {

		// skip comment lines in the file
		if (strstr(line, "#")) {
			fscanf(fpIn, "%[^\n]s", line);
		}

		// capture path to the mtl file
		if (strstr(line, "mtllib")) {
			fscanf(fpIn, "%s", line);
			strcpy(materialsPath, line);
		}

		// capturing data lines
		if (strlen(line) == 1) {

			// skipping object line
			if (strstr(line, "o")) {
				fscanf(fpIn, "%s", line);

				// capturing vertecies
			} else if (line[0] == 'v' && line[1] == '\0') {
				// checking if need to expand the size of vertecies if reached max
				if (vertCount >= sizeVerts) {
					sizeVerts *= 2;
					Vector3D *temp = realloc(verts, sizeVerts * sizeof(Vector3D));
					if (!temp) {
						printf("Error, wasn't able to reallocate verts!\n");
					}
					verts = temp;
				}

				double x, y, z;
				fscanf(fpIn, "%lf %lf %lf", &x, &y, &z);
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
					// if(facesCount>3330 && facesCount < 3350)
					// printf("(%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f)\n",
					// aa->x, aa->y, aa->z, bb->x, bb->y, bb->z, cc->x, cc->y, cc->z);

					Triangle3D trig = baseTriangle;
					trig.p1 = aa;
					trig.p2 = bb;
					trig.p3 = cc;

					Triangle3D *pTemp = (Triangle3D *)malloc(sizeof(Triangle3D));
					*pTemp = trig;
					 //printf("%4d| (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f)\n",
					 //facesCount, pTemp->p1->x, pTemp->p1->y, pTemp->p1->z, pTemp->p2->x,
						//pTemp->p2->y, pTemp->p2->z, pTemp->p3->x, pTemp->p3->y,
					 //pTemp->p3->z);
					/*if (facesCount == 93263) {
						printf("");
					}*/
					facesStack = push(facesStack, pTemp);
					facesCount++;
				}
				free(indexes);
			}
		}
	}

	for (int k = 0; k < vertCount; k++) {
		printf("%4d| (%.2f,%.2f,%.2f)\n",
			k, verts[k].x, verts[k].y, verts[k].z);
	}

	Triangle3D *triggs = (Triangle3D *)malloc(facesCount * sizeof(Triangle3D));
	int i = 0;

	divide(centerElement, vertCount, centerElement); // calculate average x,y,z

	double xMax = 0, xMin = 0;
	double yMax = 0, yMin = 0;
	double zMax = 0, zMin = 0;

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
		Triangle3D *n = facesStack->data;
		Vector3D centroid;

		if (i == 93264) {
			printf("");
		}

		// printf("%4d| (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f)\n", i,
		// n->p1->x, n->p1->y, n->p1->z, n->p2->x, n->p2->y, n->p2->z, n->p3->x,
		// n->p3->y, n->p3->z);

		if (!n || !(n->p1) || !n->p2->x || !n->p3->x) {
			printf("broke on: %d\n", i);
		}

		divide(add(add(n->p1, n->p2, &centroid), n->p3, &centroid), 2 * (*scale),
					 &centroid);
		//		n->colorR = sin(centroid.x) + .5;
		//		n->colorG = sin(centroid.y) + .5;
		//		n->colorB = sin(centroid.z) + .5;
		// funky soft colors
		n->colorR = .65 + sin(cos(centroid.y) + centroid.z) / 5;
		n->colorG = .4 + sin(cos(centroid.z) + centroid.x) / 5;
		n->colorB = .25 - sin(cos(centroid.x) + centroid.y) / 5;
		// const colors
		//		n->colorR = .15f;
		//		n->colorG = .15f;
		//		n->colorB = .15f;

		pop(&facesStack);
		triggs[i] = *n;
		i++;
	}

	*trigList = triggs;
	*size = facesCount;

	printf("\nFaces list size: %d\n", sizeFaces);
	printf("Size of the triangle: %ud\n", sizeof(Triangle3D));
	printf("Memory wasted: %ud bytes\n",
				 (sizeFaces - facesCount < 0
							? 0
							: (sizeFaces - facesCount) * sizeof(Triangle3D)));
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
	int vNum = 0;
	int *verts = (int *)malloc(5 * sizeof(int));

	char *buffer = strtok(line, "\t\n ");
	while (buffer != NULL) {
		sscanf(buffer, "%d", verts + vNum);
		vNum++;
		buffer = strtok(NULL, "\t\n ");
	}

	*vertNumber = vNum;
	*v = verts;
}

FILE *openFile(const char *path) {
	FILE *fpIn;
	if (!(fpIn = fopen(path, "r"))) {
		printf("File was not found: %s\n", path);
		exit(2);
	}
	return fpIn;
}
