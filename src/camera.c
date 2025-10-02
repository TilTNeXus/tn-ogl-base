#include "program.h"

void adjustCF(float *cf) {
    cf[0] = cosf(cam.yawangle) * cosf(cam.pitchangle);
    cf[1] = sinf(cam.pitchangle);
    cf[2] = sinf(cam.yawangle) * cosf(cam.pitchangle);
}

void cameraMovement(void) {
    float cameraSpeed = 3.0 * deltaTime;
    float cameraTurnSpeed = 2.0 * deltaTime;
    vec3f updatePos = {0.0, 0.0, 0.0};
    vec3f flatCameraFront = {cameraFront[0], 0.0, cameraFront[2]};
    vec3f_normalize(flatCameraFront);
    if (cam.forwards) {
        vec3f_scalar_multiply(updatePos, cameraSpeed, flatCameraFront);
        vec3f_add(cameraPos, cameraPos, updatePos);
    }
    if (cam.backwards) {
        vec3f_scalar_multiply(updatePos, cameraSpeed, flatCameraFront);
        vec3f_subtract(cameraPos, cameraPos, updatePos);
    }
    if (cam.left) {
        vec3f_cross(updatePos, cameraFront, cameraUp);
        vec3f_normalize(updatePos);
        vec3f_scalar_multiply(updatePos, cameraSpeed, updatePos);
        vec3f_subtract(cameraPos, cameraPos, updatePos);
    }
    if (cam.right) {
        vec3f_cross(updatePos, cameraFront, cameraUp);
        vec3f_normalize(updatePos);
        vec3f_scalar_multiply(updatePos, cameraSpeed, updatePos);
        vec3f_add(cameraPos, cameraPos, updatePos);
    }
    if (cam.up) {
        cameraPos[1] += cameraSpeed;
    }
    if (cam.down) {
        cameraPos[1] -= cameraSpeed;
    }
    if (cam.pitchu || cam.pitchd || cam.yawl || cam.yawr) {
        if (cam.pitchu) {
            cam.pitchangle += cameraTurnSpeed;
        }
        if (cam.pitchd) {
            cam.pitchangle -= cameraTurnSpeed;
        }
        if (cam.yawl) {
            cam.yawangle -= cameraTurnSpeed;
        }
        if (cam.yawr) {
            cam.yawangle += cameraTurnSpeed;
        }
        if (1.55 < cam.pitchangle) cam.pitchangle = 1.55;
        if (cam.pitchangle < -1.55) cam.pitchangle = -1.55;
        adjustCF(cameraFront);
    }
}

void billboard_model(float *m, mat4f v) {
    vec3f camRight = {v[0], v[4], v[8]};
    vec3f camUp = {v[1], v[5], v[9]};
    m[0] = camRight[0];
    m[1] = camRight[1];
    m[2] = camRight[2];
    m[3] = 0.0;
    m[4] = camUp[0];
    m[5] = camUp[1];
    m[6] = camUp[2];
    m[7] = 0.0;
    vec3f rightXUp;
    vec3f_cross(rightXUp, camRight, camUp);
    m[8] = rightXUp[0];
    m[9] = rightXUp[1];
    m[10] = rightXUp[2];
    m[11] = 0.0;
}