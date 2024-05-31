#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "defines.h"

extern GLFWwindow *window;
extern Mouse *mouse;
extern Keyboard *keyboard;

void window_init();
void window_input();