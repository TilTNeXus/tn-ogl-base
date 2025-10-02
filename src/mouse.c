#include "mouse.h"
#include "loading.h"

bool mouseInBox(mouseState ms, int left, int right, int bottom, int top) {
    return left <= ms.x && ms.x <= right && bottom <= ms.y && ms.y <= top;
}