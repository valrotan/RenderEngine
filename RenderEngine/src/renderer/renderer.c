#include "renderer.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>

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

		if (dot(&renderer->camera->dir, t->plane->v) > 0) {
			sub(&ORIGIN_3D, t->plane->v, t->plane->v);
			Vector3D *temp = t->p2;
			t->p2 = t->p3;
			t->p3 = temp;
		}
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
			ray = constructRayThroughPixel(camera, x, y, width, height);
			//			printf(" - P (%.2f, %.2f, %.2f) \n", ray->p->x, ray->p->y,
			// ray->p->z); 			printf("   V (%.2f, %.2f, %.2f) \n", ray->v->x,
			// ray->v->y, ray->v->z);

//			printf("(%.2f, %.2f) \n", ray->v->x, ray->v->y, ray->v->z);

			unsigned char *i = traceRay(camera, scene, ray, 0, 3); // 3
			*p++ = i[0];
			*p++ = i[1];
			*p++ = i[2];
			//	  printf("%d %d %d \n", i[0], i[1], i[2]);
		}
	}
}

// TODO change basically everything
Ray3D* constructRayThroughPixel(Camera* camera, int x, int y, int imageWidth, int imageHeight) {
	Ray3D* ray = (Ray3D*)malloc(sizeof(Ray3D));
	float imageAspectRatio = (float)imageWidth / (float)imageHeight; // assuming width > height 
	float scale = (float)tan(getRad(camera->screenZ / 2));

	float Px = (2 * (x + 0.5f) / (float)imageWidth - 1) * scale * imageAspectRatio;
	float Py = (1 - 2 * (y + 0.5f) / (float)imageHeight) * scale;

	Vector3D origin = { 0,0,0 };
	origin = applyTransformation(origin, camera->cameraToWorld);
	Vector3D pWorld = { Px,Py, -1 };
	pWorld = applyTransformation(pWorld, camera->cameraToWorld);

	/*printf("Printing all of the info: \n");
	printf("CASTING TO: %f %f %f\n", pWorld.x, pWorld.y, pWorld.z);
	printf("(x,y): (%d %d)\n", x, y);
	printf("PxPY : (%0.3f, %0.3f)\n\n", Px, Py);
	printf("OxOY : (%0.3f, %0.3f)\n\n", origin.x, origin.y);*/

	


	ray->p = (Vector3D*)malloc(sizeof(Vector3D));
	ray->p->x = origin.x;
	ray->p->y = origin.y;
	ray->p->z = origin.z;

	ray->v = (Vector3D*)malloc(sizeof(Vector3D));
	ray->v->x = pWorld.x - origin.x;
	ray->v->y = pWorld.y - origin.y;
	ray->v->z = pWorld.z - origin.z;
	norm(ray->v, ray->v);

	//Vec3f rayOriginWorld, rayPixelWorld;
	//cameraToWorld.multVectMatrix(rayOrigin, rayOriginWorld);
	//cameraToWorld.multVectMatrix(Vec3f(Px, Py, -1), rayPixelWorld);
	//
	//Vec3f rayDirection = rayPWorld - rayOriginWorld;
	//rayDirection.normalize(); // it's a direction so don't forget to normalize 


	/*ray->p = &camera->pos;
	ray->v = (Vector3D*)malloc(sizeof(Vector3D));
	ray->v->x = camera->dir.x;
	ray->v->y = camera->dir.y;
	ray->v->z = camera->dir.z;
	norm(ray->v, ray->v);*/
	// suppose camera is pointing at origin from +z axis


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

	return intersection->exists ? intersection : 0;
}

unsigned char *getColor(Camera *camera, Scene *scene,
												Intersection3D *intersection, int curDepth,
												int maxDepth) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

	//	printf("gc1 \n");

	float i[3];

	Triangle3D *t = intersection->triangle;

	Vector3D *normal = t->plane->v; // normal
	Vector3D view;
	sub(intersection->point, &camera->pos, &view);
	//	printf("gc2 \n");

	float ia = scene->ambientLight;
	float ie = t->k_e;

	float id = 0;
	float is = 0;

	calcPointLights(scene, intersection, normal, &view, &id, &is);
	calcDirectionalLights(scene, intersection, normal, &view, &id, &is);
	calcSpotLights(scene, intersection, normal, &view, &id, &is);

	// TODO: refraction

	// reflection
	float ir_r = 0;
	float ir_g = 0;
	float ir_b = 0;
	if (curDepth < maxDepth) {
		Vector3D reflectedVector;
		sub(&view, mul(normal, 2 * dot(&view, normal), &reflectedVector),
				&reflectedVector);
		Ray3D reflectedRay;
		Vector3D rrp, rrv;
		reflectedRay.p = &rrp;
		reflectedRay.v = &rrv;
		Vector3D eps;
		add(intersection->point, mul(normal, -.0001f, &eps),
				reflectedRay.p);               // not sure why negative
		reflectedRay.v = &reflectedVector; // might need to multiply by -1, idk

		unsigned char *reflectedColors =
				traceRay(camera, scene, &reflectedRay, curDepth + 1, maxDepth);

		ir_r = t->k_s * reflectedColors[0] / 255;
		ir_g = t->k_s * reflectedColors[1] / 255;
		ir_b = t->k_s * reflectedColors[2] / 255;
	}

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

void calcPointLights(Scene *scene, Intersection3D *intersection,
										 Vector3D *normal, Vector3D *view, float *id, float *is) {

	for (int i = 0; i < scene->nPointLights; i++) {

		PointLight *pl = &scene->pointLights[i];
		float d = dist(pl->point, intersection->point);
		Ray3D lightToInter;
		lightToInter.v = (Vector3D*)malloc(sizeof(Vector3D));
		sub(intersection->point, pl->point, lightToInter.v);
		lightToInter.p = pl->point;
		Intersection3D* inter = findIntersection(scene, &lightToInter);

		if (inter != 0 && dist(inter->point, intersection->point) < d) {
//			continue; // not count this light source
		}

		Vector3D l;
		sub(intersection->point, scene->pointLights[i].point, &l);
		norm(&l, &l);

		// check if light behind triangle
		if (dot(&l, intersection->triangle->plane->v) > 0) {
			continue;
		}

		Vector3D lightReflectedVector;
		sub(&l, mul(normal, 2 * dot(&l, normal), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D dvec = {1, d, d * d};
		float il = pl->intensity; // intensity of light
		il /= dot(pl->attenuationCoeffs, &dvec);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, &l) * il;

		// specular reflection
		float N = 3; // specular light exponent
		float is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s * powf(is_temp, N) * il;
		}
	}
}

void calcDirectionalLights(Scene *scene, Intersection3D *intersection,
													 Vector3D *normal, Vector3D *view, float *id,
													 float *is) {

	for (int i = 0; i < scene->nDirectionalLights; i++) {

		DirectionalLight *dl = &scene->directionalLights[i];

		// check if light behind triangle
		if (dot(dl->direction, intersection->triangle->plane->v) > 0) {
			continue;
		}

		Vector3D lightReflectedVector;
		sub(dl->direction, mul(normal, 2 * dot(dl->direction, normal), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		float il = dl->intensity; // intensity of light

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, dl->direction) * il;

		// specular reflection
		float N = 3; // specular light exponent
		float is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s * powf(is_temp, N) * il;
		}
	}
}

void calcSpotLights(Scene *scene, Intersection3D *intersection,
										Vector3D *normal, Vector3D *view, float *id, float *is) {

	for (int i = 0; i < scene->nSpotLights; i++) {

		SpotLight *sl = &scene->spotLights[i];

		Vector3D lightReflectedVector;
		sub(sl->direction, mul(normal, 2 * dot(sl->direction, normal), &lightReflectedVector),
				&lightReflectedVector);
		norm(&lightReflectedVector, &lightReflectedVector);

		Vector3D lightToInter;
		sub(intersection->point, sl->point, &lightToInter);

		// check if light behind triangle
		if (dot(&lightToInter, intersection->triangle->plane->v) > 0) {
			continue;
		}
		if (dot(sl->direction, intersection->triangle->plane->v) > 0) {
			continue;
		}

		float d = dist(sl->point, intersection->point);
		Vector3D dvec = {1, d, d * d};
		float il = sl->intensity; // intensity of light
		il /= dot(sl->attenuationCoeffs, &dvec);
		il *= dot(sl->direction, &lightToInter);

		// diffuse light
		*id += -intersection->triangle->k_d * dot(normal, sl->direction) * il;

		// specular reflection
		float N = 3; // specular light exponent
		float is_temp = -dot(norm(view, view), &lightReflectedVector);
		if (is_temp > 0) {
			*is += intersection->triangle->k_s * powf(is_temp, N) * il;
		}
	}
}

// TODO change the structure so the light helper functions only calculate the
// illuminosity
