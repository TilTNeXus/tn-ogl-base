#pragma once
extern int ww, wh;

#include <stdbool.h>
#include <string.h>

#ifdef EMSCRIPTEN
#include <GLES3/gl3.h>
#include <emscripten.h>
#else
#include <GL/glew.h>
#include <AL/efx.h>
#endif  

#include <AL/al.h>
#include <AL/alc.h>

#include "stb_image.h"
#include "stb_vorbis.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL3/SDL.h>

#include "mat4f.h"
#include "camera.h"
#include "text.h"
#include "conversions.h"

extern bool isGLES;
extern float deltaTime;
typedef struct cameraMove {
    bool forwards;
    bool left;
    bool right;
    bool backwards;
    bool up;
    bool down;
    bool yawr;
    bool yawl;
    bool pitchu;
    bool pitchd;
    float yawangle;
    float pitchangle;
} cameraMove;
extern cameraMove cam;
extern mat4f model;
extern mat4f view;
extern mat4f projection, projPersp, projOrtho;
extern vec3f cameraPos;
extern vec3f cameraFront;
extern vec3f cameraUp;
extern vec3f cameraPos;
extern vec3f lightPos;

typedef struct mouseState {
    float x;
    float y;
    bool down;
    bool held;
} mouseState;
extern mouseState mouse;

typedef struct keystate {
    bool down;
    bool up;
    bool held;
} keystate;
typedef struct keyboardstate {
    keystate shift;
} keyboardstate;
extern keyboardstate keys;

#include "mouse.h"

typedef struct textureinfo {
    unsigned int tex;
    unsigned int width;
    unsigned int height;
} textureinfo;

typedef struct modelinfo {
    float *verts;
    textureinfo texture;
    unsigned int size;
    unsigned int vao;
    unsigned int vbo;
} modelinfo;
extern modelinfo models[5];

typedef struct framebuffer {
    unsigned int fbo, tex, rbo, vao, vbo, shader;
} framebuffer;

typedef struct character {
    unsigned int textureID, outlineID;
    unsigned int sizeX, sizeY;
    int bearingX, bearingY;
    unsigned int advance;  
} character;
extern character characters[128];

typedef struct uiwindow {
    unsigned int shader;
    unsigned int width, height;
    float x, y;
    bool visible;
} uiwindow;

extern unsigned int textShader;
extern modelinfo textModel;

#include "loading.h"