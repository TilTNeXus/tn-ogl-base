#pragma once

#include <math.h>
#include "vec3f.h"

typedef float mat4f[16];

void mat4f_init(float *result);
void mat4f_trans_multiply(float *result, float matA[16], float matB[16]);
void mat4f_trans_translate(float *mat, float x, float y, float z);
void mat4f_trans_rotate(float *mat, float angle, float x, float y, float z);
void mat4f_trans_scale(float *mat, float x, float y, float z);
void mat4f_trans_perspective(float *mat, float fov, float aspect, float znear, float zfar);
void mat4f_trans_orthographic(float *mat, float left, float right, float bottom, float top, float znear, float zfar);
void mat4f_lookat(float *mat, float eye[3], float center[3], float up[3]);