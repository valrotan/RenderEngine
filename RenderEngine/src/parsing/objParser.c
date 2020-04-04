#include "objParser.h"

void parseObj(const char *path, Triangle3D **trigList, int *size, double *scale,
			  Triangle3D baseTriangle, int useMTL) {
	FILE *fpIn = openFile(path);

	char materialsPath[100]; // for future material path parsing

	char line[1000]; // buffer line

	int sizeVerts = 1000; // the current size of vertex array
	Vector3D *verts = (Vector3D *)malloc(sizeVerts * sizeof(Vector3D));
	int vertCount = 0;

	int sizeFaces = 1000;
	StackNode *facesStack = NULL;
	Vector3D *centerElement =
		(Vector3D *)malloc(sizeof(Vector3D)); // used for centering vertecies
	centerElement->x = 0;
	centerElement->y = 0;
	centerElement->z = 0;

	unsigned int nMaterials = 0, curMat = 0;
	Material *materials;

	int facesCount = 0;
	int *lineCount = (int *)malloc(sizeof(int));
	*lineCount = 0;

	while (fscanf(fpIn, "%s", line) != EOF) {
		(*lineCount)++;
		// skip comment lines in the file
		if (strstr(line, "#")) {
			fscanf(fpIn, "%[^\n]s", line);
		}

		// capture path to the mtl file
		if (strstr(line, "mtllib")) {
			fscanf(fpIn, "%s", line);
			strcpy(materialsPath, line);
			printf("%s \n", materialsPath);
			if (useMTL) {
				char *mtlPath = (char *)malloc((strlen(path) + strlen(materialsPath)) *
											   sizeof(char));
				strcpy(mtlPath, path);
				if (strchr(mtlPath, '/')) {
					strcpy(strrchr(mtlPath, '/') + 1, materialsPath);
				} else {
					strcpy(strrchr(mtlPath, '\\') + 1, materialsPath);
				}
				materials = parseMtl(mtlPath, baseTriangle, &nMaterials);
				if (materials == NULL) {
					printf("Failed to load materials\n");
					useMTL = 0;
				}
			}
		}

		// capture current material being used
		if (strstr(line, "usemtl") && useMTL) {
			fscanf(fpIn, "%s", line);
			curMat = findMaterial(materials, nMaterials, line);
			if (curMat == -1) {
				printf("Material not found: %s; using first materials \n", line);
				curMat = 0;
			}
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

				int *indexes, vertNum = 0;
				if (strlen(line) >= 300) {
					printf("%s\n", line);
				}

				// parsing the face line to create multiple triangles if more
				// than 3 vertecies
				parseFaceLine(line, &vertNum, &indexes);

				int first = indexes[0];
				for (int i = 1; i + 1 < vertNum; i++) {
					facesStack = push(facesStack, first - 1, indexes[i] - 1,
									  indexes[i + 1] - 1, curMat);
					facesCount++;
				}
				free(indexes);
			}
		}
	}

	printf("Finished parsing\n");

	// for (int k = 0; k < vertCount; k++) {
	//	printf("%4d| (%.2f,%.2f,%.2f)\n",
	//		k, verts[k].x, verts[k].y, verts[k].z);
	//}

	Triangle3D *triggs = (Triangle3D *)malloc(facesCount * sizeof(Triangle3D));

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

	int i = 0;

	while (facesStack && i < facesCount) {
		// printf("%d %d %d\n", n.p1, n.p2, n.p3);

		Triangle3D *trig = (Triangle3D *)malloc(sizeof(Triangle3D));
		StackNode *stackNode = pop(&facesStack);

		if (useMTL) {
			*trig = materials[stackNode->curMat].baseTriangle;
		} else {
			*trig = baseTriangle;
		}

		trig->p1 = *(verts + stackNode->p1);
		trig->p2 = *(verts + stackNode->p2);
		trig->p3 = *(verts + stackNode->p3);

		free(stackNode);

		divide(add(add(&trig->p1, &trig->p2, &trig->centroid), &trig->p3,
				   &trig->centroid),
			   2 * (*scale), &trig->centroid);
		if (!useMTL) {
			//		n->colorR = sin(centroid.x) + .5;
			//		n->colorG = sin(centroid.y) + .5;
			//		n->colorB = sin(centroid.z) + .5;
			// funky soft colors
			//		trig->colorR = .2 + sin(cos(trig->centroid.y) + trig->centroid.z) /
			//5; 		trig->colorG = .2 + sin(cos(trig->centroid.z) + trig->centroid.x) /
			//5; 		trig->colorB = .75 - sin(cos(trig->centroid.x) + trig->centroid.y) /
			// 5;

			//		trig->colorR = .25 - sin(trig->centroid.x - trig->centroid.y) / 5;
			//		trig->colorG = .25 - sin(trig->centroid.x - trig->centroid.y) / 5;
			//		trig->colorB = .25 - sin(trig->centroid.x - trig->centroid.y) / 5;

			trig->colorR = .3;
			trig->colorG = .7;
			trig->colorB = 1;
		}

		triggs[i] = *trig;
		i++;
	}

	*trigList = triggs;
	*size = facesCount;

	printf("\nFaces list size: %d\n", sizeFaces);
	printf("Size of the triangle: %u\n", sizeof(Triangle3D));
	printf("Memory wasted: %ud bytes\n",
		   (sizeFaces - facesCount < 0
				? 0
				: (sizeFaces - facesCount) * sizeof(Triangle3D)));
	printf("Number of verts: %d\n", vertCount);
	printf("Number of faces: %d\n\n", facesCount);

	line[0] = '\0';

	free(centerElement);
	free(facesStack);
	if (useMTL) {
		free(materials);
	}

	if (fclose(fpIn) != 0) {
		exit(3);
	}
}

void parseFaceLine(char *line, int *vertNumber, int **v) {
	int vNum = 0;
	int maxVerts = 120;

	int *verts = (int *)malloc(maxVerts * sizeof(int));

	char *buffer = strtok(line, "\t\n ");
	while (buffer != NULL) {
		if (vNum >= maxVerts) {
			maxVerts *= 2;
			int *temp = (int *)realloc(verts, maxVerts * sizeof(int));
			verts = temp;
		}
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

int materialCompareByKey(const void *a, const void *b) {
	const Material *ma = (const Material *)a;
	const Material *mb = (const Material *)b;
	return strcmp(ma->key, mb->key);
}

int findMaterial(Material *mats, int nMats, const char *key) {
	Material temp;
	temp.key = key;
	Material *found = (Material *)bsearch(&temp, mats, nMats, sizeof(Material),
										  materialCompareByKey);
	return found - mats;
}

Material *parseMtl(const char *path, Triangle3D baseTriangle,
				   unsigned int *nMaterials) {
	FILE *mtlFile = fopen(path, "r");
	if (!mtlFile) {
		printf("Failed to open mtl file: %s \n", path);
		return NULL;
	}

	unsigned int nMats = 0;
	Material *materials = (Material *)malloc(2 * sizeof(Material));

	size_t len = 0;
	char *line = NULL;
	while (getline(&line, &len, mtlFile) != -1) {
		char *word = strtok(line, " \t\n\r");
		if (!word)
			continue;
		if (strcmp("newmtl", word) == 0) {
			// create new material
			if (nMats != 0 && (nMats & (nMats - 1)) == 0) { // if size is a power of 2
				materials = realloc(materials, nMats * 2 * sizeof(Material));
			}
			materials[nMats].baseTriangle = baseTriangle;
			word = strtok(NULL, " \t\n\r");
			materials[nMats].key = (char *)malloc((strlen(word) + 1) * sizeof(char));
			strcpy(materials[nMats].key, word);
			nMats++;

		} else if (strcmp("Ka", word) == 0) {
			// set color of current material
			materials[nMats - 1].baseTriangle.colorR =
				strtod(strtok(NULL, " \t\n\r"), NULL);
			materials[nMats - 1].baseTriangle.colorG =
				strtod(strtok(NULL, " \t\n\r"), NULL);
			materials[nMats - 1].baseTriangle.colorB =
				strtod(strtok(NULL, " \t\n\r"), NULL);
		}
	}
	fclose(mtlFile);

	qsort(materials, nMats, sizeof(Material), materialCompareByKey);
	*nMaterials = nMats;

	return materials;
}

void deleteMaterials(Material **pMats, int nMats) {
	Material *mats = *pMats;
	for (int i = 0; i < nMats; i++) {
		free(mats->key);
	}
	free(mats);
	*pMats = NULL;
}
