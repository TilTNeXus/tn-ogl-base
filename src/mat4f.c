#include "mat4f.h"

void mat4f_init(float *result) {
    for (int i = 0; i < 16; i++) {
        if (i % 5 == 0) result[i] = 1.0;
        else result[i] = 0.0;
    }
}

void mat4f_trans_multiply(float *result, float matA[16], float matB[16]) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result[row*4+col] = 0.0f;
            for (int mult = 0; mult < 4; mult++) {
                result[4*row+col] += matA[4*row+mult]*matB[4*mult+col];
            }
        }
    }
}

void mat4f_trans_translate(float *mat, float x, float y, float z) {
    mat4f result;
    mat4f trans = {1,0,0,0,0,1,0,0,0,0,1,0,x,y,z,1};
    mat4f_trans_multiply(result, mat, trans);
    for (int i = 0; i < 16; i++) {
        mat[i] = result[i];
    }
}

void mat4f_trans_rotate(float *mat, float angle, float x, float y, float z) {
    mat4f result;
    vec3f normaxes = {x, y, z};
    vec3f_normalize(normaxes);
    x = normaxes[0];
    y = normaxes[1];
    z = normaxes[2];
    float ca = cosf(angle);
    float omca = 1.0 - ca;
    float sa = sinf(angle);
    mat4f trans = { ca+x*x*omca,    y*x*omca+z*sa,  z*x*omca-y*sa,  0,
                    x*y*omca-z*sa,  ca+y*y*omca,    z*y*omca+x*sa,  0,
                    x*z*omca+y*sa,  y*z*omca-x*sa,  ca+z*z*omca,    0,
                    0,              0,              0,              1
    };
    mat4f_trans_multiply(result, mat, trans);
    for (int i = 0; i < 16; i++) {
        mat[i] = result[i];
    }
}

void mat4f_trans_scale(float *mat, float x, float y, float z) {
    mat4f result;
    mat4f trans = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
    mat4f_trans_multiply(result, mat, trans);
    for (int i = 0; i < 16; i++) {
        mat[i] = result[i];
    }
}

void mat4f_trans_perspective(float *mat, float fov, float aspect, float znear, float zfar) {
    for (int i = 0; i < 16; i++) {
        mat[i] = 0.0;
    }
    float tanHalf = tanf(fov / 2.0);
    mat[0] = 1.0 / (aspect * tanHalf);
    mat[5] = 1.0 / tanHalf;
    mat[10] = -(zfar + znear) / (zfar - znear);
    mat[11] = -1.0;
    mat[14] = -2.0 * zfar * znear / (zfar - znear);
}

void mat4f_trans_orthographic(float *mat, float left, float right, float bottom, float top, float znear, float zfar) {
    for (int i = 0; i < 16; i++) {
        mat[i] = 0.0;
    }
    mat[0] = 2.0 / (right - left);
    mat[5] = 2.0 / (top - bottom);
    mat[10] = -2.0 / (zfar - znear);
    mat[12] = -(right + left) / (right - left);
    mat[13] = -(top + bottom) / (top - bottom);
    mat[14] = -(zfar + znear) / (zfar - znear);
    mat[15] = 1.0;
}

void mat4f_lookat(float *mat, float eye[3], float center[3], float up[3]) {
    vec3f f, s, u;
    vec3f_subtract(f, center, eye);
    vec3f_normalize(f);
    vec3f_cross(s, f, up);
    vec3f_normalize(s);
    vec3f_cross(u, s, f);

    for (int i = 0; i < 16; i++) {
        mat[i] = 0;
    }
    mat[0]  =  s[0];
    mat[4]  =  s[1];
    mat[8]  =  s[2];
    mat[1]  =  u[0];
    mat[5]  =  u[1];
    mat[9]  =  u[2];
    mat[2]  = -f[0];
    mat[6]  = -f[1];
    mat[10] = -f[2];
    mat[12] = -vec3f_dot(s, eye);
    mat[13] = -vec3f_dot(u, eye);
    mat[14] =  vec3f_dot(f, eye);
    mat[15] = 1;
}