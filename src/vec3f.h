#pragma once

#include <math.h>

typedef float vec3f[3];

void vec3f_add(float *vec, vec3f a, vec3f b);
void vec3f_subtract(float *vec, vec3f a, vec3f b);
void vec3f_cross(float *vec, vec3f a, vec3f b);
float vec3f_dot(vec3f a, vec3f b);
void vec3f_scalar_multiply(float *vec, float scalar, vec3f a);
void vec3f_normalize(float *vec);