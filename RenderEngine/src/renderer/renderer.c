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
	t->plane->d = -dot(t->p1, t->plane->v);
	printf("init triangle : V (%.2f, %.2f, %.2f) d %.2f \n", t->plane->v->x,
		   t->plane->v->y, t->plane->v->z, t->plane->d);
	printf("  color : C (%d, %d, %d) \n", t->colorR, t->colorG, t->colorB);
	printf("  refl  : C (%.2f, %.2f, %.2f) \n", t->k_d, t->k_e, t->k_s);
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

	  unsigned char *i = traceRay(camera, scene, ray, 0, 0);
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
  ray->v = norm(ray->v);
  return ray;
}

unsigned char *traceRay(Camera *camera, Scene *scene, Ray3D *ray, int curDepth,
						int maxDepth) {

  Intersection3D *intersection = findIntersection(scene, ray);
  unsigned char *color;
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
	  tempDist = dist(tempIntersection->point, ray->p);
	  float pos = dot(ray->v, sub(tempIntersection->point, ray->p));
	  //	  tempDist = dot(ray->v, sub(tempIntersection->point, ray->p));
//	  printf("(%.2f, %.2f, %.2f) %.2f\n", tempIntersection->point->x,
//			 tempIntersection->point->y, tempIntersection->point->z, tempDist);
	  if (tempDist < minDist) { // not sure why pos < 0
		free(intersection);
		intersection = tempIntersection;
		minDist = tempDist;
	  } else {
		free(tempIntersection);
	  }
	}
  }
  //	if (intersection != 0) {
  //		Vector3D *r = intersection->point;
  //		printf("found intersection (%f %f %f)\n", r->x, r->y, r->z);
  //	}
  return intersection;
}

unsigned char *getColor(Camera *camera, Scene *scene,
						Intersection3D *intersection, int curDepth,
						int maxDepth) {
  // I = Ie + Ka Ial + Kd (N * L) Il + Ks (V * R)^n Ii

  float i[3];

  Triangle3D *t = intersection->triangle;

  float ia = scene->ambientLight;
  float ie = t->k_e;

  // for each light source:
  // intersection to light
  // diffuse light
  Vector3D *l = sub(intersection->point, scene->pointLights->point);
  l = norm(l);
  Vector3D *n = t->plane->v; // normal
  float id = -t->k_d * dot(n, l) * scene->pointLights->intensity;

  // specular reflection
  Vector3D *v = sub(intersection->point, &camera->pos);
  // r=d-2(dot(dn))n
  Vector3D *lightReflectedVector = sub(l, mul(n, 2 * dot(l, n)));
  float N = 3; // specular light exponent
  float is = -dot(norm(v), norm(lightReflectedVector));
  if (is > 0) {
	is = t->k_s * powf(is, N) * scene->pointLights->intensity;
  } else {
	is = 0;
  }
  // endfor

  // TODO light blocking
  // if light is blocked (intersection with scene w/ dist < len(ray))
  // ia + id = 0

  // TODO: refraction
  // reflection
  float ir_r = 0;
  float ir_g = 0;
  float ir_b = 0;
  if (curDepth < maxDepth) {
	Vector3D *view = norm(mul(v, -1));
	Vector3D *reflectedVector = sub(view, mul(n, 2 * dot(view, n)));
	Ray3D reflectedRay;
	reflectedRay.p = add(intersection->point,
						 mul(norm(n), -.0001f)); // not sure why negative
	reflectedRay.v = mul(reflectedVector, -1);

	//		printf("v (%.2f, %.2f, %.2f)\n", v->x, v->y, v->z);
	//		printf("r (%.2f, %.2f, %.2f)\n", reflectedRay.v->x,
	// reflectedRay.v->y, reflectedRay.v->z);

	unsigned char *reflectedColors =
		traceRay(camera, scene, &reflectedRay, curDepth + 1, maxDepth);
	//		printf("%f\n", reflectedColor);
	ir_r = t->k_s * reflectedColors[0] / 255;
	ir_g = t->k_s * reflectedColors[1] / 255;
	ir_b = t->k_s * reflectedColors[2] / 255;
  }
  //	ia = 0; id = 0; is = 0;
  i[0] = t->colorR * (ia + id + ie) / 255.0f + is + ir_r;
  i[1] = t->colorG * (ia + id + ie) / 255.0f + is + ir_g;
  i[2] = t->colorB * (ia + id + ie) / 255.0f + is + ir_b;


  printf("color: (%d %d %d) -> (%.2f %.2f %.2f) \n", t->colorR, t->colorG, t->colorB, ia, id, ie);


  //  printf("inter: %.2f %.2f %.2f \n", intersection->point->x, intersection->point->y, intersection->point->z);
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
  //	if (i != 0) {
  //		printf("%f\n", i);
  //	}

  return i_char;
}
