#pragma once

#include "cglm/cglm.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define CAMERA_SPEED 3.0f

#define MOUSE_SENSITIVITY 0.1f

typedef struct Camera
{
    vec3 position;
    vec3 front;
    vec3 up;

    float fov;
    float yaw;
    float pitch;
} Camera;

typedef struct Renderer
{
    float delta_time;
    float last_frame_time;
} Renderer;

typedef struct Mouse
{
    float x;
    float y;
    bool first_move_handled;
} Mouse;

typedef struct Keyboard
{
    bool w_pressed;
    bool a_pressed;
    bool s_pressed;
    bool d_pressed;
} Keyboard;
