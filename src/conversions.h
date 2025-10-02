#pragma once

#include "program.h"

void hexToRGBf(float *rgbf, unsigned int hex);
void RGBtoHSL(float *h, float *s, float *l, float r, float g, float b);
void HSLtoRGB(float *r, float *g, float *b, float h, float s, float l);
char *base64Encode(int srclen, unsigned char *src, int *destlen);