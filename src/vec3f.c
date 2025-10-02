#include "vec3f.h"

void vec3f_add(float *vec, vec3f a, vec3f b) {
    vec[0] = a[0] + b[0];
    vec[1] = a[1] + b[1];
    vec[2] = a[2] + b[2];
}

void vec3f_subtract(float *vec, vec3f a, vec3f b) {
    vec[0] = a[0] - b[0];
    vec[1] = a[1] - b[1];
    vec[2] = a[2] - b[2];
}

void vec3f_cross(float *vec, vec3f a, vec3f b) {
    vec[0] = a[1]*b[2] - a[2]*b[1];
    vec[1] = a[2]*b[0] - a[0]*b[2];
    vec[2] = a[0]*b[1] - a[1]*b[0];
}

float vec3f_dot(vec3f a, vec3f b) {
    float product = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    return product;
}

void vec3f_scalar_multiply(float *vec, float scalar, vec3f a) {
    for (int i = 0; i < 3; i++) {
        vec[i] = scalar*a[i];
    }
}

void vec3f_normalize(float *vec) {
    float x = vec[0];
    float y = vec[1];
    float z = vec[2];
    float magnitude = sqrtf(x*x + y*y + z*z);
    vec[0] = x/magnitude;
    vec[1] = y/magnitude;
    vec[2] = z/magnitude;
}