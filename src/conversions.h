#pragma once

#include "program.h"

unsigned int moduint(int x, int y);
float moduf(float x, float y);
void hexToRGBf(float *rgbf, unsigned int hex);
void RGBtoHSL(float *h, float *s, float *l, float r, float g, float b);
void HSLtoRGB(float *r, float *g, float *b, float h, float s, float l);
char *base64Encode(int srclen, unsigned char *src, int *destlen);
unsigned char *base64Decode(int srclen, unsigned char *src, int *destlen);