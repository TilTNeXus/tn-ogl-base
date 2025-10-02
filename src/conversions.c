#include "conversions.h"

void hexToRGBf(float *rgbf, unsigned int hex) {
    rgbf[0] = (float)(hex >> 16) / 255;
    rgbf[1] = (float)((hex >> 8) & 0xff) / 255;
    rgbf[2] = (float)(hex & 0xff) / 255;
}

void RGBtoHSL(float *h, float *s, float *l, float r, float g, float b) {
    float cmax = 0.0;
    float cmin = 2.0;
    float deltac;
    if (cmax < r) cmax = r;
    if (cmax < g) cmax = g;
    if (cmax < b) cmax = b;
    if (cmin > r) cmin = r;
    if (cmin > g) cmin = g;
    if (cmin > b) cmin = b;
    deltac = cmax - cmin;

    *l = (cmax + cmin) / 2.0;
    if (deltac == 0) {
        *h = 0;
        *s = 0;
    } else if (cmax == r) {
        *h = (moduf((g - b)/deltac, 6.0))/6.0;
    } else if (cmax == g) {
        *h = (((b - r)/deltac) + 2.0)/6.0;
    } else if (cmax == b) {
        *h = (((r - g)/deltac) + 4.0)/6.0;
    }
    if (deltac != 0) {
        *s = deltac / (1 - fabs(2.0 * *l - 1));
    }
}

void HSLtoRGB(float *r, float *g, float *b, float h, float s, float l) {
    h *= 6.0;
    float c = (1 - fabs(2.0 * l - 1)) * s;
    float x = c * (1.0 - fabs(fmodf(h, 2.0) - 1.0));
    float m = l - c/2.0;
    
    if (h < 1) {
        *r = c;
        *g = x;
        *b = 0.0;
    } else if (1 <= h && h < 2) {
        *r = x;
        *g = c;
        *b = 0.0;
    } else if (2 <= h && h < 3) {
        *r = 0.0;
        *g = c;
        *b = x;
    } else if (3 <= h && h < 4) {
        *r = 0.0;
        *g = x;
        *b = c;
    } else if (4 <= h && h < 5) {
        *r = x;
        *g = 0.0;
        *b = c;
    } else {
        *r = c;
        *g = 0.0;
        *b = x;
    }
    *r += m;
    *g += m;
    *b += m;
}

char *base64Encode(int srclen, unsigned char *src, int *destlen) {
    char table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int destLength = 4*((srclen+2)/3) + 2;
    char *dest = malloc(destLength * sizeof(char));
    int padding = 0;
    int out = 0;
    for (int i = 0; i < srclen; i += 3) {
        int bits;
        padding = 0;
        if ((i+2) < srclen) {
            bits = (src[i] << 16) + (src[i+1] << 8) + src[i+2];
        } else if ((i+1) < srclen) {
            bits = (src[i] << 16) + (src[i+1] << 8);
            padding = 1;
        } else {
            bits = (src[i] << 16);
            padding = 2;
        }
        for (int j = 0; j < 4; j++) {
            int shift = 18 - 6*j;
            int map = (bits & (0b111111 << shift)) >> shift;
            dest[4*out+j] = table[map];
        }
        if (padding == 1) dest[4*i+3] = '=';
        if (padding == 2) dest[4*i+2] = '=';
        out++;
    }
    dest[destLength] = '\0';
    *destlen = destLength;
    return dest;
}