#include "renderer.h"
#include <GLFW/glfw3.h>

Renderer *renderer;

void renderer_init()
{
    renderer = malloc(sizeof(Renderer));
    renderer->delta_time = 0.0f;
    renderer->last_frame_time = glfwGetTime();
}

void renderer_update()
{
    float current_frame_time = glfwGetTime();
    renderer->delta_time = current_frame_time - renderer->last_frame_time;
    renderer->last_frame_time = current_frame_time;
}