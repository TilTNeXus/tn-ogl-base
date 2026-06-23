#pragma once

#include "program.h"

unsigned int shaderLoad(const char* vsPath, const char* fsPath);
void textureLoad(textureinfo *texture, const char* texPath, unsigned int format, bool flipped);
void modelLoad(modelinfo *dest, const char* modelPath);
void modelLoadTextured(modelinfo *dest, const char* modelPath, const char* texPath);
unsigned int modelLoadOBJ(modelinfo *dest, const char* modelPath);
void modelLoadGLTF(modelinfo *dest, const char *modelPath);
void createFramebuffer(framebuffer *fb, int width, int height, int offsetX);
void loadIntoVAO(modelinfo *model);
void loadUniformsAndDraw(modelinfo mdl, unsigned int shader);
void loadUniformsAndDrawIndexed(modelinfo mdl, unsigned int shader);
void createRectFromTexture(modelinfo *mdl, textureinfo texture);
void orthoMode();

enum modelFormat {
    MODEL_OBJ = 0, 
    MODEL_GLTF = 1
};