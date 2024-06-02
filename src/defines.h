#pragma once

#include "cglm/cglm.h"
#include "gfx.h"

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

    bool wireframe_enabled;

    GLuint program_id;

    GLuint ibo_id;
    GLuint vbo_id;
    GLuint vao_id;

    GLuint model_location;
    GLuint view_location;
    GLuint projection_location;
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

typedef struct Window
{
    GLFWwindow *handle;
    int width, height;
    Mouse mouse;
    Keyboard keyboard;
} Window;

typedef struct State
{
    Window window;
    Renderer renderer;
    Camera camera;
} State;

extern struct State state;
