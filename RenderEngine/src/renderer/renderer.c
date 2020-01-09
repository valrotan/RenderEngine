#include "renderer.h"
#include "float.h"
#include "math.h"
#include "stdlib.h"

void rendererInit(Renderer *renderer) {
	for (int i = 0; i < renderer->scene->nTriangles; i++) {
		Triangle3D *t = &renderer->scene->triangles[i];
		Vector3D *v1 = sub(t->p1, t->p3);
		Vector3D *v2 = sub(t->p1, t->p2);
		t->plane = (Plane3D *)malloc(sizeof(Plane3D));
		t->plane->v = cross(v1, v2);
		t->plane->v = norm(t->plane->v);
		t->plane->d = dot(v1, t->plane->v);
	}
}

void rayCast(Camera *camera, Scene *scene, unsigned char *screen, int width,
						 int height) {
	Ray3D *ray;
	unsigned char *p = screen;

	int halfWidth = width / 2;
	int halfHeight = height / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			ray = constructRayThroughPixel(camera, x - halfWidth, y - halfHeight);

			// start temp code
			//			if (findIntersection(scene, ray) != 0) {
			//				*p++ = 255;
			//				*p++ = 255;
			//				*p++ = 255;
			//			} else {
			//				*p++ = 0;
			//				*p++ = 0;
			//				*p++ = 0;
			//			}
			//			free(ray);
			// end temp code
			float i = traceRay(camera, scene, ray, 0, 5);
			unsigned char temp;
			if (i > 1) {
				temp = 255;
			} else if (i < 0) {
				temp = 0;
			} else {
				temp = (unsigned char)(i * 255);
			}
			*p++ = temp;
			*p++ = temp;
			*p++ = temp;
			//   find intersection
			//   get color + trace ray (for each intersection bounced ray)
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
	ray->v->y = y;
	ray->v->z = -camera->screenZ;
	return ray;
}

Intersection3D *findIntersection(Scene *scene, Ray3D *ray) {

	Intersection3D *intersection = 0;
	Intersection3D *tempIntersection;
	float minDist = FLT_MAX;
	float tempDist;

	for (int i = 0; i < scene->nTriangles; i++) {
		tempIntersection = intersect(ray, &scene->triangles[i]);
		//		Vector3D *r = tempIntersection->point;
		//		printf("%f %f %f\n", r->x, r->y, r->z);
		if (tempIntersection != 0) {
			//			printf("%f\n", dist(&ray->p,
			// tempIntersection->point));
			if ((tempDist = dist(ray->p, tempIntersection->point)) < minDist) {
				free(intersection);
				intersection = tempIntersection;
				minDist = tempDist;
			}
		}
	}
	//	if (intersection != 0) {
	//		Vector3D *r = intersection->point;
	//		printf("found intersection (%f %f %f)\n", r->x, r->y, r->z);
	//	}
	return intersection;
}

float traceRay(Camera *camera, Scene *scene, Ray3D *ray, int curDepth,
							 int maxDepth) {

	Intersection3D *intersection = findIntersection(scene, ray);
	float color;
	if (intersection != 0) {
		color = getColor(camera, scene, intersection, curDepth, maxDepth);
	} else {
		color = .15; // background light
	}
	free(intersection);
	return color;
	//	return 0;
}

float getColor(Camera *camera, Scene *scene, Intersection3D *intersection,
							 int curDepth, int maxDepth) {
	// I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

	float i = 0;

	float ia = .25;
	float ie = intersection->triangle->lightEmission;

	// for each light source:
	// intersection to light
	// diffuse light
	Vector3D *l = sub(intersection->point, scene->pointLights->point);
	l = norm(l);
	Vector3D *n = intersection->triangle->plane->v; // normal
	float K_D = .125;
	float id = -K_D * dot(n, l) * scene->pointLights->intensity;

	// specular reflection
	Vector3D *v = sub(&camera->pos, intersection->point);
	// r=d-2(dot(dn))n
	Vector3D *lightReflectedVector = sub(l, mul(n, 2 * dot(l, n)));
	float K_S = .25;
	float N = 3;
	float is = dot(norm(v), norm(lightReflectedVector));
	if (is > 0) {
		is = K_S * pow(is, N) * scene->pointLights->intensity;
	} else {
		is = 0;
	}
	// endfor

	// if light is blocked (intersection with scene w/ dist < len(ray))
	// ia + id = 0

	// TODO: refraction
	// reflection
	float ir = 0;
	if (curDepth < maxDepth) {
		Vector3D *view = mul(v, -1);
		Vector3D *reflectedVector = sub(view, mul(n, 2 * dot(view, n)));
		Ray3D reflectedRay;
		reflectedRay.p = add(intersection->point, mul(norm(n), -.0001)); // not sure why negative
		reflectedRay.v = reflectedVector;

		//		printf("v (%.2f, %.2f, %.2f)\n", v->x, v->y, v->z);
		//		printf("r (%.2f, %.2f, %.2f)\n", reflectedRay.v->x,
		//reflectedRay.v->y, 					 reflectedRay.v->z);

		float reflectedColor =
				traceRay(camera, scene, &reflectedRay, curDepth + 1, maxDepth);
		//		printf("%f\n", reflectedColor);
		ir = K_S * reflectedColor;
	}
	//	ir = 0;
	i = ia + id + is + ie + ir;

	//	if (i != 0) {
	//		printf("%f\n", i);
	//	}

	return i;
}
