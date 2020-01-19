#include "renderer.h"
#include "float.h"
#include "math.h"
#include "stdlib.h"

void rendererInit(Renderer *renderer) {
	for (int i = 0; i < renderer->scene->nTriangles; i++) {
		Triangle3D *t = &renderer->scene->triangles[i];
		Vector3D v1;
		sub(t->p1, t->p3, &v1);
		Vector3D v2;
		sub(t->p1, t->p2, &v2);
		t->plane = (Plane3D *)malloc(sizeof(Plane3D));
		t->plane->v = (Vector3D *)malloc(sizeof(Vector3D));
		cross(&v1, &v2, t->plane->v);
		norm(t->plane->v, t->plane->v);
		t->plane->d = -dot(t->p1, t->plane->v);
		printf("init triangle : N (%.2f, %.2f, %.2f) d %.2f \n", t->plane->v->x,
					 t->plane->v->y, t->plane->v->z, t->plane->d);
		printf("  color : C (%d, %d, %d) \n", t->colorR, t->colorG, t->colorB);
		printf("  refl  : C (%.2f, %.2f, %.2f) \n", t->k_d, t->k_e, t->k_s);
	}
}

void rayTrace(Camera *camera, Scene *scene, unsigned char *screen, int width,
							int height) {

	Ray3D *ray;
	unsigned char *p = screen;

	int halfWidth = width / 2;
	int halfHeight = height / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			ray = constructRayThroughPixel(camera, x - halfWidth, y - halfHeight);
//			printf(" - P (%.2f, %.2f, %.2f) \n", ray->p->x, ray->p->y, ray->p->z);
//			printf("   V (%.2f, %.2f, %.2f) \n", ray->v->x, ray->v->y, ray->v->z);
			unsigned char *i = traceRay(camera, scene, ray, 0, 3);
			*p++ = i[0];
			*p++ = i[1];
			*p++ = i[2];
			//	  printf("%d %d %d \n", i[0], i[1], i[2]);
		}
	}
}

// TODO change basically everything
Ray3D *constructRayThroughPixel(Camera *camera, int x, int y) {
	Ray3D *ray = (Ray3D *)malloc(sizeof(Ray3D));
	ray->p = &camera->pos;
	// ray->v = camera->dir; // for (0, 0)

	// suppose camera is pointing at origin from +z axis
	ray->v = (Vector3D *)malloc(sizeof(Vector3D));
	ray->v->x = x;
	ray->v->y = y; // proper up would be -y
	ray->v->z = -camera->screenZ;
	norm(ray->v, ray->v);
	return ray;
}

unsigned char *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int curDepth,
												int maxDepth) {

	Intersection3D *intersection = findIntersection(scene, ray);
	unsigned char *color;
//	printf("tr1 \n");
	if (intersection != 0) {
		color = getColor(camera, scene, intersection, curDepth, maxDepth);
	} else {
		color = (unsigned char *)malloc(sizeof(char) * 3);
		color[0] = scene->bkgR; // background light
		color[1] = scene->bkgG; // background light
		color[2] = scene->bkgB; // background light
	}
	free(intersection);
	return color;
	//	return 0;
}

Intersection3D *copyIntersection(Intersection3D *o, Intersection3D *i) {

	o->point->x = i->point->x;
	o->point->y = i->point->y;
	o->point->z = i->point->z;
	o->triangle = i->triangle;
	o->originalRay = i->originalRay;
	o->exists = i->exists;

	return o;
}

Intersection3D *findIntersection(Scene *scene, Ray3D *ray) {

	Intersection3D *intersection =
			(Intersection3D *)malloc(sizeof(Intersection3D));
	intersection->point = (Vector3D *)malloc(sizeof(Vector3D));
	intersection->exists = 0;

	Intersection3D *tempIntersection =
			(Intersection3D *)malloc(sizeof(Intersection3D));
	tempIntersection->point = (Vector3D *)malloc(sizeof(Vector3D));

	float minDist = FLT_MAX;
	float tempDist;

	for (int i = 0; i < scene->nTriangles; i++) {
		tempIntersection->exists = 1;
		intersect(ray, &scene->triangles[i], tempIntersection);

		if (tempIntersection->exists) {
			tempDist = dist(tempIntersection->point, ray->p);
			if (tempDist < minDist) {
				copyIntersection(intersection, tempIntersection);
				minDist = tempDist;
			}
		}
	}
	//	if (intersection != 0) {
	//		Vector3D *r = intersection->point;
	//		printf("found intersection (%f %f %f)\n", r->x, r->y, r->z);
	//	}
	return intersection->exists ? intersection : 0;
}

unsigned char *getColor(Camera *camera, Scene *scene,
												Intersection3D *intersection, int curDepth,
												int maxDepth) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

//	printf("gc1 \n");

	float i[3];

	Triangle3D *t = intersection->triangle;

	Vector3D *n = t->plane->v; // normal
	Vector3D v;
	sub(intersection->point, &camera->pos, &v);
//	printf("gc2 \n");

	float ia = scene->ambientLight;
	float ie = t->k_e;

	float id = 0;
	float is = 0;

//	printf("gc3 \n");

	// point lights
	for (int i = 0; i < scene->nPointLights; i++) {

		Vector3D l;
		sub(intersection->point, scene->pointLights[i].point, &l);
		norm(&l, &l);
		Vector3D lightReflectedVector;
		sub(&l, mul(n, 2 * dot(&l, n), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		PointLight *pl = &scene->pointLights[i];
		float d = dist(pl->point, intersection->point);
		Vector3D dvec = {1, d, d * d};
		float il = pl->intensity; // intensity of light
		il /= dot(pl->attenuationCoeffs, &dvec);

		// diffuse light
		id += -t->k_d * dot(n, &l) * il;

		// specular reflection
		float N = 3; // specular light exponent
		float is_temp = -dot(norm(&v, &v), &lightReflectedVector);
		if (is_temp > 0) {
			is += t->k_s * powf(is_temp, N) * il;
		}
	}

//	printf("gc4 \n");

	// TODO light blocking
	// if light is blocked (intersection with scene w/ dist < len(ray))
	// id = 0

	// TODO: refraction
	// reflection
	float ir_r = 0;
	float ir_g = 0;
	float ir_b = 0;
	if (curDepth < maxDepth) {
		Vector3D reflectedVector;
		sub(&v, mul(n, 2 * dot(&v, n), &reflectedVector), &reflectedVector);
		Ray3D reflectedRay;
		Vector3D rrp, rrv;
		reflectedRay.p = &rrp;
		reflectedRay.v = &rrv;
		Vector3D eps;
		add(intersection->point, mul(n, -.0001f, &eps),
				reflectedRay.p);               // not sure why negative
		reflectedRay.v = &reflectedVector; // might need to multiply by -1, idk

		unsigned char *reflectedColors =
				traceRay(camera, scene, &reflectedRay, curDepth + 1, maxDepth);

		ir_r = t->k_s * reflectedColors[0] / 255;
		ir_g = t->k_s * reflectedColors[1] / 255;
		ir_b = t->k_s * reflectedColors[2] / 255;
	}
//	printf("gc5 \n");
	i[0] = t->colorR * (ia + id + ie) / 255.0f + is + ir_r;
	i[1] = t->colorG * (ia + id + ie) / 255.0f + is + ir_g;
	i[2] = t->colorB * (ia + id + ie) / 255.0f + is + ir_b;

	unsigned char *i_char = (unsigned char *)malloc(sizeof(char) * 3);
	if (i[0] > 1) {
		i_char[0] = 255;
	} else if (i[0] < 0) {
		i_char[0] = 0;
	} else {
		i_char[0] = (unsigned char)(i[0] * 255);
	}
	if (i[1] > 1) {
		i_char[1] = 255;
	} else if (i[1] < 0) {
		i_char[1] = 0;
	} else {
		i_char[1] = (unsigned char)(i[1] * 255);
	}
	if (i[2] > 1) {
		i_char[2] = 255;
	} else if (i[2] < 0) {
		i_char[2] = 0;
	} else {
		i_char[2] = (unsigned char)(i[2] * 255);
	}

	return i_char;
}
