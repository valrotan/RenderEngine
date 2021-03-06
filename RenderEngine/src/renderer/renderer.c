#include "renderer.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#define EPSILON .0000001

void rendererInit(Renderer *renderer) {

	Camera *cam = renderer->camera;
	cam->scale = tan(getRad(cam->fov / 2));
	cam->aspectRatio = (double)(cam->width) / cam->height;

	for (int i = 0; i < renderer->scene->nTriangles; i++) {
		Triangle3D *t = &renderer->scene->triangles[i];
		Vector3D v1;
		sub(&t->p1, &t->p3, &v1);
		Vector3D v2;
		sub(&t->p1, &t->p2, &v2);

		cross(&v1, &v2, &t->plane.v);
		norm(&t->plane.v, &t->plane.v);
		t->plane.d = -dot(&t->p1, &t->plane.v);

		divide(add(add(&t->p1, &t->p2, &t->centroid), &t->p3, &t->centroid), 3,
			   &t->centroid);
	}
	BoundingVolume *bv = malloc(sizeof(BoundingVolume));
	// convert scene triangles to double pointers to triangles
	Triangle3D **triangles =
		malloc(sizeof(Triangle3D *) * (size_t)renderer->scene->nTriangles);
	for (int i = 0; i < renderer->scene->nTriangles; i++) {
		triangles[i] = renderer->scene->triangles + i;
	}
	bv->triangles = triangles;
	bv->nTriangles = renderer->scene->nTriangles;

	//	printf("init construction \n");
	renderer->scene->bv = constructBoundingVolumes(bv);
	printf("finished construction \n");

	//	printf("%d \n", bv->nChildren);
	//	printf("%d \n", bv->children->nChildren);
	//	printBoundingVolume(bv);
	//	printf("low  (%.2f, %.2f, %.2f) \n", bv->low.x, bv->low.y, bv->low.z);
	//	printf("high (%.2f, %.2f, %.2f) \n", bv->high.x, bv->high.y, bv->high.z);
}

void *rayTraceSegment(void *pSegment) {

	RendererSegment *rSegment = (RendererSegment *)pSegment;

	Ray3D ray; // can move this outside
	applyTransformation(&ORIGIN_3D, &rSegment->renderer->camera->cameraToWorld,
						&ray.p);

	unsigned char *p = rSegment->screen;

	double rgb[3] = {0};
	int ns = rSegment->renderer->nAntialiasingSamples;
	for (int y = rSegment->startLine; y < rSegment->stopLine; y++) {
		for (int x = 0; x < rSegment->width; x++) {
			double temp_rgb[3] = {0, 0, 0};

			for (int i = 0; i < ns; i++) {
				for (int j = 0; j < ns; j++) {
					constructRayThroughPixel(rSegment->renderer->camera, ns * x + i,
											 ns * y + j, rSegment->width,
											 rSegment->height, &ray, ns);
					traceRay(rSegment->renderer->camera, rSegment->renderer->scene, &ray,
							 rSegment->renderer->nTraces, rgb);

					temp_rgb[0] += rgb[0];
					temp_rgb[1] += rgb[1];
					temp_rgb[2] += rgb[2];
					//					temp_rgb[0] += sigmoid(4 * rgb[0] - 2);
					//					temp_rgb[1] += sigmoid(4 * rgb[1] - 2);
					//					temp_rgb[2] += sigmoid(4 * rgb[2] - 2);
				}
			}

			rgb[0] = temp_rgb[0] / (double)(ns * ns);
			rgb[1] = temp_rgb[1] / (double)(ns * ns);
			rgb[2] = temp_rgb[2] / (double)(ns * ns);

			//			constructRayThroughPixel(rSegment->renderer->camera, x, y,
			// rSegment->width, rSegment->height, &ray, 1);

			//			traceRay(rSegment->renderer->camera, rSegment->renderer->scene,
			//&ray, 							 rSegment->renderer->nTraces, rgb);

			if (rgb[0] > 1) {
				*p++ = 255;
			} else if (rgb[0] < 0) {
				*p++ = 0;
			} else {
				*p++ = (unsigned char)(rgb[0] * 255);
			}
			if (rgb[1] > 1) {
				*p++ = 255;
			} else if (rgb[1] < 0) {
				*p++ = 0;
			} else {
				*p++ = (unsigned char)(rgb[1] * 255);
			}
			if (rgb[2] > 1) {
				*p++ = 255;
			} else if (rgb[2] < 0) {
				*p++ = 0;
			} else {
				*p++ = (unsigned char)(rgb[2] * 255);
			}
		}
	}
	return NULL;
}

// TODO: split in grid maybe to utilize threads more
// Fun thing to try: plot execution time of thread vs line number to see
//     how threads are utilized when split this way
void rayTrace(Renderer *renderer, unsigned char *screen) {

	pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * //
											 (size_t)renderer->nThreads);
	RendererSegment *rSegments = (RendererSegment *)malloc(
		sizeof(RendererSegment) * (size_t)renderer->nThreads);
	double lines = (double)(renderer->camera->height) / renderer->nThreads;

	for (int i = 0; i < renderer->nThreads; i++, rSegments++) {
		rSegments->renderer = renderer;
		rSegments->screen =
			screen + 3 * renderer->camera->width * (int)(i * lines + .5);
		rSegments->width = renderer->camera->width;
		rSegments->height = renderer->camera->height;
		rSegments->startLine = (int)(i * lines + .5);
		rSegments->stopLine = (int)((i + 1) * lines + .5);
		pthread_create(threads + i, NULL, rayTraceSegment, (void *)rSegments);
	}
	for (int i = 0; i < renderer->nThreads; i++) {
		pthread_join(threads[i], NULL);
	}
	free(threads);
}

// Constructs a ray that originates at the camera position and shoots through
// the given pixel (x,y)
// MBP performance: ~50ms for HD on single threadb
Ray3D *constructRayThroughPixel(Camera *camera, int x, int y, int imageWidth,
								int imageHeight, Ray3D *ray,
								int samplesPerPixel) {

	// TODO: move all the constants to the loop in rayTrace
	double Px = (2 * ((double)x / samplesPerPixel) / imageWidth - 1) *
				camera->scale * camera->aspectRatio;
	double Py =
		(1 - 2 * ((double)y / samplesPerPixel) / imageHeight) * camera->scale;

	Vector3D pWorld = {Px, Py, -1};
	applyTransformation(&pWorld, &camera->cameraToWorld, &pWorld);

	sub(&pWorld, &ray->p, &ray->v);
	norm(&ray->v, &ray->v);

	return ray;
}

// recursive ray tracing for depth levels
// TODO: prevent excess dynamic allocation for pixel
double *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int depth,
				 double *rgb) {

	Intersection3D intersection;
	intersection.exists = 0;

	findIntersectionBV(scene->bv, ray, &intersection);
	intersection.originalRay = ray;

	if (intersection.exists) {
		getColor(camera, scene, &intersection, depth, rgb);
	} else {
		rgb[0] = scene->bkgR; // background light
		rgb[1] = scene->bkgG; // background light
		rgb[2] = scene->bkgB; // background light
	}
	return rgb;
}

void swap(Triangle3D **a, Triangle3D **b) {
	Triangle3D *t = *a;
	*a = *b;
	*b = t;
}

// Quick split: original credit Geeks for Geeks
// heavily modified
/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
	array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition(Triangle3D **arr, int low, int high,
			  double getAxis(Triangle3D *)) {
	Triangle3D *pivot = arr[low + rand() % (high + 1 - low)]; // pivot
	int i = (low - 1); // Index of smaller element

	double cPivot = getAxis(pivot);

	for (int j = low; j <= high - 1; j++) {

		double cj = getAxis(arr[j]);
		// If current element is smaller than the pivot
		if (cj < cPivot) {
			i++; // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

Triangle3D *kthSmallest(Triangle3D **arr, int l, int r, int k,
						double getAxis(Triangle3D *)) {
	// If k is smaller than number of
	// elements in array
	if (k > 0 && k <= r - l + 1) {

		// Partition the array around last
		// element and get position of pivot
		// element in sorted array
		int index = partition(arr, l, r, getAxis);

		// If position is same as k
		if (index - l == k - 1)
			return arr[index];

		// If position is more, recur
		// for left subarray
		if (index - l > k - 1)
			return kthSmallest(arr, l, index - 1, k, getAxis);

		// Else recur for right subarray
		return kthSmallest(arr, index + 1, r, k - index + l - 1, getAxis);
	}

	// If k is more than number of
	// elements in array
	return 0;
}

int quickPartition(Triangle3D **arr, int low, int high,
				   double getAxis(Triangle3D *)) {
	Triangle3D *pivot =
		kthSmallest(arr, low, high, high / 2, getAxis); // pivot at median
	int i = (low - 1); // Index of smaller element

	double cPivot = getAxis(pivot);

	for (int j = low; j <= high - 1; j++) {
		double cj = getAxis(arr[j]);
		// If current element is smaller than the pivot
		if (cj < cPivot) {
			i++; // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

void constructVolumeBounds(BoundingVolume *bv) {
	Triangle3D *cur = bv->triangles[0];

	bv->low.x = fmin(fmin(cur->p1.x, cur->p2.x), cur->p3.x);
	bv->low.y = fmin(fmin(cur->p1.y, cur->p2.y), cur->p3.y);
	bv->low.z = fmin(fmin(cur->p1.z, cur->p2.z), cur->p3.z);
	bv->high.x = fmax(fmax(cur->p1.x, cur->p2.x), cur->p3.x);
	bv->high.y = fmax(fmax(cur->p1.y, cur->p2.y), cur->p3.y);
	bv->high.z = fmax(fmax(cur->p1.z, cur->p2.z), cur->p3.z);

	for (int i = 1; i < bv->nTriangles; i++) {
		cur = bv->triangles[i];
		bv->low.x = fmin(bv->low.x, fmin(fmin(cur->p1.x, cur->p2.x), cur->p3.x));
		bv->low.y = fmin(bv->low.y, fmin(fmin(cur->p1.y, cur->p2.y), cur->p3.y));
		bv->low.z = fmin(bv->low.z, fmin(fmin(cur->p1.z, cur->p2.z), cur->p3.z));
		bv->high.x = fmax(bv->high.x, fmax(fmax(cur->p1.x, cur->p2.x), cur->p3.x));
		bv->high.y = fmax(bv->high.y, fmax(fmax(cur->p1.y, cur->p2.y), cur->p3.y));
		bv->high.z = fmax(bv->high.z, fmax(fmax(cur->p1.z, cur->p2.z), cur->p3.z));
	}
}

double getVolume(BoundingVolume *bv) {
	return (bv->high.x - bv->low.x) * //
		   (bv->high.y - bv->low.y) * //
		   (bv->high.z - bv->low.z);
}

double getSurfaceArea(BoundingVolume *bv) {
	Vector3D dims;
	sub(&bv->high, &bv->low, &dims);
	return (dims.x * dims.y) + //
		   (dims.x * dims.z) + //
		   (dims.y - dims.z);
}

void swapd(double *a, double *b) {
	double temp = *b;
	*b = *a;
	*a = temp;
}

void vectorSort(Vector3D *v) {
	if (v->y < v->x)
		swapd(&v->x, &v->y);

	// Insert arr[2]
	if (v->z < v->y) {
		swapd(&v->y, &v->z);
		if (v->y < v->x)
			swapd(&v->y, &v->x);
	}
}

double volumeQuantifier(BoundingVolume *bv) {
	Vector3D dims;
	sub(&bv->high, &bv->low, &dims);
	vectorSort(&dims);
	return (dims.x + dims.y + 2 * dims.z);
}

// generate bounding volumes
// in: scene
// create node around all triangles
// if width > height
//   find Triangle3Dnumber n/2 x-wise
//   child 1 = triangles 0 to n/2
//   child 2 = triangles n/2 to n
//   recurse into children
// else
//   find Triangle3Dnumber n/2 y-wise
//   child 1 = triangles 0 to n/2
//   child 2 = triangles n/2 to n
//   recurse into children
BoundingVolume *constructBoundingVolumes(BoundingVolume *bv) {

	if (bv->nTriangles == 0) {
		bv->low = ORIGIN_3D;
		bv->high = ORIGIN_3D;
		bv->nChildren = 0;
		return bv;
	}
	// determine own bounding volume points
	constructVolumeBounds(bv);
	bv->nChildren = 0;

	// end at n triangles per volume
	if (bv->nTriangles > 2) {
		int nLow = bv->nTriangles / 2;
		int nHigh = bv->nTriangles - nLow;

		Triangle3D **low =
			(Triangle3D **)malloc(sizeof(Triangle3D *) * (size_t)nLow);
		Triangle3D **high = (Triangle3D **)malloc( //
			sizeof(Triangle3D *) * (size_t)nHigh);

		Vector3D bvDims;
		sub(&bv->high, &bv->low, &bvDims);
		//		printf("dims (%.2f, %.2f, %.2f) \n", bvDims.x, bvDims.y, bvDims.z);

		BoundingVolume *children =
			(BoundingVolume *)malloc(sizeof(BoundingVolume) * 2);
		low = bv->triangles;
		high = bv->triangles + nLow;
		children[0].triangles = low;
		children[0].nTriangles = nLow;
		children[1].triangles = high;
		children[1].nTriangles = nHigh;
		bv->children = children;
		bv->nChildren = 2;

		{
			// Pick dimension based on min total children surface area
			//			double (*quantifier)(BoundingVolume *) = volumeQuantifier;
			//			char pick = 'x';
			//			double minQ, q;
			//			quickPartition(bv->triangles, 0, bv->nTriangles - 1,
			//triangleCentroidX);
			//			constructVolumeBounds(children);
			//			constructVolumeBounds(children + 1);
			//			minQ = quantifier(children) + quantifier(children + 1);

			//			Triangle3D **byY = malloc(sizeof(Triangle3D *) *
			//(size_t)bv->nTriangles);
			//			memcpy(byY, bv->triangles, sizeof(Triangle3D *) *
			//(size_t)bv->nTriangles);
			//			children[0].triangles = byY;
			//			children[1].triangles = byY + nLow;
			//			quickPartition(byY, 0, bv->nTriangles - 1, triangleCentroidY);
			//			constructVolumeBounds(children);
			//			constructVolumeBounds(children + 1);
			//			q = quantifier(children) + quantifier(children + 1);
			//			if (q < minQ) {
			//				minQ = q;
			//				pick = 'y';
			//			}

			//			Triangle3D **byZ = malloc(sizeof(Triangle3D *) *
			//(size_t)bv->nTriangles);
			//			memcpy(byZ, bv->triangles, sizeof(Triangle3D *) *
			//(size_t)bv->nTriangles);
			//			children[0].triangles = byZ;
			//			children[1].triangles = byZ + nLow;
			//			quickPartition(byZ, 0, bv->nTriangles - 1, triangleCentroidZ);
			//			constructVolumeBounds(children);
			//			constructVolumeBounds(children + 1);
			//			q = quantifier(children) + quantifier(children + 1);
			//			if (q < minQ) {
			//				pick = 'z';
			//			}

			//			// could just store the sorts instead to speed up construction
			//			if (pick == 'x') { // split along x
			//				children[0].triangles = bv->triangles;
			//				children[1].triangles = bv->triangles + nLow;
			//			} else if (pick == 'y') { // split along y
			//				children[0].triangles = byY;
			//				children[1].triangles = byY + nLow;
			//			}
		}

		{
			if (bvDims.x > bvDims.y && bvDims.x > bvDims.z) { // split along x
				quickPartition(bv->triangles, 0, bv->nTriangles - 1, triangleCentroidX);
			} else if (bvDims.y > bvDims.z) { // split along y
				quickPartition(bv->triangles, 0, bv->nTriangles - 1, triangleCentroidY);
			} else { // split along z
				quickPartition(bv->triangles, 0, bv->nTriangles - 1, triangleCentroidZ);
			}
		}

		//		memcpy(low, bv->triangles, sizeof(Triangle3D *) * (size_t)nLow);
		//		memcpy(high, bv->triangles + nLow, sizeof(Triangle3D *) *
		//(size_t)nHigh);

		bv->nTriangles = 0;

		constructBoundingVolumes(children);
		constructBoundingVolumes(children + 1);
	}

	return bv;
}

// inspiration: http://www.cs.utah.edu/~awilliam/box/box.pdf
int smitsBoxIntersect(BoundingVolume *bv, Ray3D *ray) {
	double tmin, tmax, tymin, tymax, tzmin, tzmax;
	double divx = 1 / ray->v.x;
	if (divx >= 0) {
		tmin = (bv->low.x - ray->p.x) * divx;
		tmax = (bv->high.x - ray->p.x) * divx;
	} else {
		tmin = (bv->high.x - ray->p.x) * divx;
		tmax = (bv->low.x - ray->p.x) * divx;
	}
	double divy = 1 / ray->v.y;
	if (divy >= 0) {
		tymin = (bv->low.y - ray->p.y) * divy;
		tymax = (bv->high.y - ray->p.y) * divy;
	} else {
		tymin = (bv->high.y - ray->p.y) * divy;
		tymax = (bv->low.y - ray->p.y) * divy;
	}
	if ((tmin > tymax) || (tymin > tmax))
		return 0;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	double divz = 1 / ray->v.z;
	if (divz >= 0) {
		tzmin = (bv->low.z - ray->p.z) * divz;
		tzmax = (bv->high.z - ray->p.z) * divz;
	} else {
		tzmin = (bv->high.z - ray->p.z) * divz;
		tzmax = (bv->low.z - ray->p.z) * divz;
	}
	if ((tmin > tzmax) || (tzmin > tmax))
		return 0;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return (tmax > 0);
}

// if intersects bounding volume:
// if bv.nTriangles > 0
//   find min intersection with triangles
// if bv.nChildren > 0
//   find min intersection with bounding volumes
// return min
Intersection3D *findIntersectionBV(BoundingVolume *bv, Ray3D *ray,
								   Intersection3D *inter) {

	if (smitsBoxIntersect(bv, ray)) {

		Intersection3D tempIntersection;
		tempIntersection.exists = 0;

		double minDist = DBL_MAX;
		double tempDist;

		// Triangle3Dintersections
		for (int i = 0; i < bv->nTriangles; i++) {
			intersect(ray, *(bv->triangles + i), &tempIntersection);

			if (tempIntersection.exists) {
				tempDist = distSqrd(&tempIntersection.point, &ray->p);
				if (tempDist < minDist) {
					inter->point = tempIntersection.point;
					inter->triangle = *(bv->triangles + i);
					inter->exists = 1;
					minDist = tempDist;
				}
			}
		}

		// children intersections
		for (int i = 0; i < bv->nChildren; i++) {
			findIntersectionBV(bv->children + i, ray, &tempIntersection);
			if (tempIntersection.exists) {
				tempDist = distSqrd(&tempIntersection.point, &ray->p);
				if (tempDist < minDist) {
					inter->point = tempIntersection.point;
					inter->triangle = tempIntersection.triangle;
					inter->exists = 1;
					minDist = tempDist;
				}
			}
		}

		return inter;
	}
	inter->exists = 0;
	return inter;
}

// gets a color from a specific intersection and calls rayTrace recursively
// handles all light in the program
double *getColor(Camera *camera, Scene *scene, Intersection3D *intersection,
				 int depth, double *rgb) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

	Triangle3D *t = intersection->triangle;

	Vector3D normal = t->plane.v; // normal
	if (dot(&intersection->originalRay->v, &normal) > 0) {
		sub(&ORIGIN_3D, &normal, &normal);
	}
	Vector3D *view = &intersection->originalRay->v;

	double id = 0;
	double is = 0;

	calcPointLights(scene, intersection, &normal, view, &id, &is);
	calcDirectionalLights(scene, intersection, &normal, view, &id, &is);
	calcSpotLights(scene, intersection, &normal, view, &id, &is);

	// TODO: refraction/transparency

	// reflection
	double ir_r = 0;
	double ir_g = 0;
	double ir_b = 0;
	if (depth > 0) {
		Vector3D reflectedVector;
		sub(view, mul(&normal, 2 * dot(view, &normal), &reflectedVector),
			&reflectedVector);
		Ray3D reflectedRay;
		add(&intersection->point, mul(&normal, EPSILON, &reflectedRay.v),
			&reflectedRay.p);
		reflectedRay.v = reflectedVector;

		double reflectedColors[3];
		traceRay(camera, scene, &reflectedRay, depth - 1, reflectedColors);

		ir_r = t->k_s * reflectedColors[0];
		ir_g = t->k_s * reflectedColors[1];
		ir_b = t->k_s * reflectedColors[2];
	}

	rgb[0] = t->colorR * (scene->ambientLight + id + t->k_e) + is + ir_r;
	rgb[1] = t->colorG * (scene->ambientLight + id + t->k_e) + is + ir_g;
	rgb[2] = t->colorB * (scene->ambientLight + id + t->k_e) + is + ir_b;

	return rgb;
}

void calcPointLights(Scene *scene, Intersection3D *intersection,
					 Vector3D *normal, Vector3D *view, double *id, double *is) {

	for (int i = 0; i < scene->nPointLights; i++) {

		PointLight *pl;
		pl = &(scene->pointLights[i]);

		double d = dist(&intersection->point, pl->point);

		// shadows
		Ray3D lightToInter;
		sub(&intersection->point, pl->point, &lightToInter.v);
		norm(&lightToInter.v, &lightToInter.v);

		double lightNormalDot = dot(&lightToInter.v, normal);
		// check if light behind triangle
		if (lightNormalDot > 0) { // light points into triangle
			continue;
		}

		lightToInter.p = *pl->point;

		// shadow
		Intersection3D inter;
		inter.exists = 0;
		findIntersectionBV(scene->bv, &lightToInter, &inter);
		if (inter.exists && dist(&inter.point, pl->point) < d - EPSILON) {
			continue; // not count this light source
		}

		Vector3D lightReflectedVector;
		sub(&lightToInter.v, mul(normal, 2 * lightNormalDot, &lightReflectedVector),
			&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D dvec = {1, d, d * d};
		double il = pl->intensity; // intensity of light
		il /= dot(pl->attenuationCoeffs, &dvec);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, &lightToInter.v) * il;

		// specular reflection
		double is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s *
				   pow(is_temp, scene->kSpecularExponent) * il;
		}
	}
}

void calcDirectionalLights(Scene *scene, Intersection3D *intersection,
						   Vector3D *normal, Vector3D *view, double *id,
						   double *is) {

	for (int i = 0; i < scene->nDirectionalLights; i++) {

		DirectionalLight *dl = &scene->directionalLights[i];

		// check if light behind triangle
		if (dot(dl->direction, normal) > 0) {
			continue;
		}

		// shadows
		Ray3D lightToInter;
		mul(dl->direction, -1, &lightToInter.v);
		add(&intersection->point, mul(normal, EPSILON, &lightToInter.p),
			&lightToInter.p);

		Intersection3D inter;
		inter.exists = 0;
		findIntersectionBV(scene->bv, &lightToInter, &inter);
		if (inter.exists) {
			continue; // not count this light source
		}

		Vector3D lightReflectedVector;
		sub(dl->direction,
			mul(normal, 2 * dot(dl->direction, normal), &lightReflectedVector),
			&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		double il = dl->intensity; // intensity of light

		// diffuse light (add negative due to vector orientations)
		*id -= intersection->triangle->k_d * dot(normal, dl->direction) * il;

		// specular reflection
		double is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s *
				   pow(is_temp, scene->kSpecularExponent) * il;
		}
	}
}

void calcSpotLights(Scene *scene, Intersection3D *intersection,
					Vector3D *normal, Vector3D *view, double *id, double *is) {

	for (int i = 0; i < scene->nSpotLights; i++) {

		SpotLight *sl = &scene->spotLights[i];
		double d = dist(sl->point, &intersection->point);

		Vector3D lightReflectedVector;
		sub(sl->direction,
			mul(normal, 2 * dot(sl->direction, normal), &lightReflectedVector),
			&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D lightToInter;
		sub(&intersection->point, sl->point, &lightToInter);

		// check if light behind triangle
		if (dot(&lightToInter, normal) > 0 || dot(sl->direction, normal) > 0) {
			continue;
		}

		// shadows
		Ray3D lightToInterRay;
		lightToInterRay.v = lightToInter;
		lightToInterRay.p = *sl->point;

		Intersection3D inter;
		inter.exists = 0;
		findIntersectionBV(scene->bv, &lightToInterRay, &inter);
		if (inter.exists && dist(&inter.point, sl->point) < d - EPSILON) {
			continue; // not count this light source
		}

		Vector3D dvec = {1, d, d * d};
		double il = sl->intensity; // intensity of light
		il /= dot(sl->attenuationCoeffs, &dvec);
		il *= dot(sl->direction, &lightToInter);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, sl->direction) * il;

		// specular reflection
		double is_temp = -dot(view, &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s *
				   pow(is_temp, scene->kSpecularExponent) * il;
		}
	}
}
