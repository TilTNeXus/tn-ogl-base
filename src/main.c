#define SDL_MAIN_USE_CALLBACKS
#define STB_IMAGE_IMPLEMENTATION
#define STB_VORBIS_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL_main.h>

#include "program.h"

static SDL_Window *window = NULL;
SDL_GLContext context = NULL;
ALCdevice *aldevice = NULL;
ALCcontext *alcontext = NULL;
bool running;
Uint64 ticks;
float deltaTime = 0.0;
bool isGLES = false;
int ww = 640;
int wh = 480;

mouseState mouse;
keyboardstate keys;

cameraMove cam;

framebuffer sceneFB;
framebuffer uiFB;
framebuffer textFB;
unsigned int shaderProgram[5];
modelinfo models[5];

unsigned int textShader;
modelinfo textModel;
character characters[128];

mat4f model;
mat4f view;
mat4f projection, projPersp, projOrtho;
vec3f cameraPos = {0.0, 0.0, 3.0};
vec3f cameraFront = { 0.0, 0.0, -1.0};
vec3f cameraUp = {0.0, 1.0, 0.0};
vec3f lightPos = {0.0, 2.0, 2.0};

unsigned int alsource, albuffer;

SDL_AppResult SDL_AppEvent (void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            running = 0;
            return SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.key) {
                case SDLK_W:
                    cam.forwards = 1;
                    break;
                case SDLK_A:
                    cam.left = 1;
                    break;
                case SDLK_S:
                    cam.backwards = 1;
                    break;
                case SDLK_D:
                    cam.right = 1;
                    break;
                case SDLK_SPACE:
                    cam.up = 1;
                    break;
                case SDLK_LSHIFT:
                    cam.down = 1;
                    break;
                case SDLK_LEFT:
                    cam.yawl = 1;
                    break;
                case SDLK_RIGHT:
                    cam.yawr = 1;
                    break;
                case SDLK_UP:
                    cam.pitchu = 1;
                    break;
                case SDLK_DOWN:
                    cam.pitchd = 1;
                    break;
                default:
                    break;
            }
            break;
        case SDL_EVENT_KEY_UP:
            switch (event->key.key) {
                case SDLK_W:
                    cam.forwards = 0;
                    break;
                case SDLK_A:
                    cam.left = 0;
                    break;
                case SDLK_S:
                    cam.backwards = 0;
                    break;
                case SDLK_D:
                    cam.right = 0;
                    break;
                case SDLK_SPACE:
                    cam.up = 0;
                    break;
                case SDLK_LSHIFT:
                    cam.down = 0;
                    break;
                case SDLK_LEFT:
                    cam.yawl = 0;
                    break;
                case SDLK_RIGHT:
                    cam.yawr = 0;
                    break;
                case SDLK_UP:
                    cam.pitchu = 0;
                    break;
                case SDLK_DOWN:
                    cam.pitchd = 0;
                    break;
                default:
                    break;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            SDL_GetMouseState(&mouse.x, &mouse.y);
            mouse.down = 1;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouse.down = 0;
            break;    
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

void initDrawing(void) {

    textShader = shaderLoad("assets/shaders/text.vs", "assets/shaders/text.fs");
    glUseProgram(textShader);
    glUniform1i(glGetUniformLocation(textShader, "text"), 0);
    loadFont("assets/fonts/ChicagoFLF.ttf", wh/24);
    glGenVertexArrays(1, &textModel.vao);
    glGenBuffers(1, &textModel.vbo);
    glBindVertexArray(textModel.vao);
    glBindBuffer(GL_ARRAY_BUFFER, textModel.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindVertexArray(0);

    mat4f_init(projOrtho);
    mat4f_init(projPersp);
    mat4f_trans_orthographic(projOrtho, 0.0, ww, 0.0, wh, 0.1, 100.0);
    mat4f_trans_perspective(projPersp, M_PI / 2.0, (float)ww/wh, 0.1, 100.0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    createFramebuffer(&sceneFB, ww, wh, 0);
    glUniform1i(glGetUniformLocation(sceneFB.shader, "tex"), 0);

    createFramebuffer(&uiFB, ww, wh, 0);
    glUniform1i(glGetUniformLocation(uiFB.shader, "tex"), 0);

    createFramebuffer(&textFB, ww, wh, 0);
    glUniform1i(glGetUniformLocation(textFB.shader, "tex"), 0);


    modelLoad(&models[0], "assets/models/fish.obj");
    loadIntoVAO(&models[0]);
    shaderProgram[0] = shaderLoad("assets/shaders/shape.vs", "assets/shaders/solidlit.fs");
    vec3f objectColor;
    hexToRGBf(objectColor, 0x1587d1);
    vec3f lightColor = {1.0, 1.0, 1.0};
    glUseProgram(shaderProgram[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram[0], "objectColor"), 1, objectColor);
    glUniform3fv(glGetUniformLocation(shaderProgram[0], "lightColor"), 1, lightColor);

}

void initAudio(void) {
    alGenSources(1, &alsource);
    alSourcef(alsource, AL_PITCH, 1);
    alSourcef(alsource, AL_GAIN, 0.25);
    alSource3f(alsource, AL_POSITION, 0, 0, 0);
    alSource3f(alsource, AL_VELOCITY, 0, 0, 0);
    alSourcei(alsource, AL_LOOPING, AL_FALSE);
    alSourcef(alsource, AL_REFERENCE_DISTANCE, 1);
    alSourcef(alsource, AL_MAX_DISTANCE, 50);
    alSourcef(alsource, AL_ROLLOFF_FACTOR, 1);

    short *output;
    int channels, sampleRate, samples;
    samples = stb_vorbis_decode_filename("assets/sounds/fish.ogg", &channels, &sampleRate, &output);
    if (!samples) {
        printf("Failed to load\n");
        return;
    }
    ALenum format;
    if (channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        free(output);
        return;
    }

    alGenBuffers(1, &albuffer);
    alBufferData(albuffer, format, output, samples * channels * sizeof(short), sampleRate);
    free(output);
    alSourcei(alsource, AL_BUFFER, albuffer);

    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    
}

void renderScene(void) {
    cameraMovement();
    alListener3f(AL_POSITION, cameraPos[0], cameraPos[1], cameraPos[2]);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    float orientation[6] = {cameraFront[0], cameraFront[1], cameraFront[2], cameraUp[0], cameraUp[1], cameraUp[2]};
    alListenerfv(AL_ORIENTATION, orientation);
    vec3f viewCenter;
    vec3f_add(viewCenter, cameraPos, cameraFront);
    mat4f_lookat(view, cameraPos, viewCenter, cameraUp);

    memcpy(projection, projPersp, sizeof(mat4f));

    glCullFace(GL_BACK);

    mat4f_init(model);
    mat4f_trans_scale(model, 0.2, 0.2, 0.2);
    //mat4f_trans_translate(model, 0.0, 1.0, 0.0);
    mat4f_trans_rotate(model, (float)ticks/250, 0.0, 1.0, 0.0);
    loadUniformsAndDraw(models[0], shaderProgram[0]);
}

void renderUI(void) {
    orthoMode();

    glUseProgram(textFB.shader);
    glBindTexture(GL_TEXTURE_2D, textFB.tex);
    glBindVertexArray(textFB.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFB.fbo);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    renderScene();

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, uiFB.fbo);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderUI();

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    
    glUseProgram(sceneFB.shader);
    glBindTexture(GL_TEXTURE_2D, sceneFB.tex);
    /*
    float caAmountX = 0;
    float caAmountY = 0;
    glUniform1f(glGetUniformLocation(fb.shader, "caAmountX"), caAmountX);
    glUniform1f(glGetUniformLocation(fb.shader, "caAmountY"), caAmountY);
    float hue = (float)((ticks / 20) % 360 ) / 360;
    vec3f filterColor;
    HSLtoRGB(&filterColor[0], &filterColor[1], &filterColor[2], hue, 0.25, 1.0);
    glUniform3fv(glGetUniformLocation(fb.shader, "filterColor"), 1, filterColor);
    glUniform1f(glGetUniformLocation(fb.shader, "filterAmount"), 1.0);
    glUniform1f(glGetUniformLocation(fb.shader, "time"), (float)ticks/1000);
    */
    glBindVertexArray(sceneFB.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUseProgram(uiFB.shader);
    glBindTexture(GL_TEXTURE_2D, uiFB.tex);
    glBindVertexArray(uiFB.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    SDL_GL_SwapWindow(window);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    running = 1;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    window = SDL_CreateWindow("base", ww, wh, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    aldevice = alcOpenDevice(NULL);
    alcontext = alcCreateContext(aldevice, NULL);
    alcMakeContextCurrent(alcontext);

    #ifdef EMSCRIPTEN
    isGLES = true;
    #else 
    glewInit();
    #endif
    
    initDrawing();
    initAudio();
    SDL_srand(SDL_GetTicks());
    cam.yawangle = -M_PI_2;
    cam.pitchangle = 0.0;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    Uint64 currentTicks = SDL_GetTicks();
    deltaTime = (float)(currentTicks - ticks) / 1000;
    ticks = currentTicks;
    draw();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(alcontext);
    alcCloseDevice(aldevice);
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}