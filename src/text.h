#pragma once
#include "program.h"

void loadFont(const char *fontpath, unsigned int size);
void drawText(unsigned int shader, char *text, float x, float y, float scale, float color[3], float shadowcolor[3], float offset);
void drawTextParametric(unsigned int shader, char *text, float x, float y, float scale, float color[3], float shadowcolor[3], float offset);