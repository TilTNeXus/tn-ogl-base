#pragma once

#include "program.h"

unsigned int moduint(int x, int y);
float moduf(float x, float y);
unsigned int shaderLoad(const char* vsPath, const char* fsPath);
void textureLoad(textureinfo *texture, const char* texPath, unsigned int format);
unsigned int modelLoad(modelinfo *dest, const char* modelPath);
void createFramebuffer(framebuffer *fb, int width, int height, int offsetX);
void loadIntoVAO(modelinfo *model);
void loadUniformsAndDraw(modelinfo mdl, unsigned int shader);
void loadUniformsAndDrawTextured(modelinfo mdl, unsigned int shader);
void createRectFromTexture(modelinfo *mdl, textureinfo texture);
void orthoMode();