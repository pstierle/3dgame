#include "window.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include "camera.h"
#include "renderer.h"
#include "util.h"

GLFWwindow *window;
Mouse *mouse;
Keyboard *keyboard;

void window_mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    if (mouse->first_move_handled == false)
    {
        mouse->x = x_pos;
        mouse->y = y_pos;
        mouse->first_move_handled = true;
    }

    camera_mouse_move(mouse->x, x_pos, mouse->y, y_pos);

    mouse->x = x_pos;
    mouse->y = y_pos;
}

void window_scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    camera_mouse_scroll((float)y_offset);
}

void window_init()
{
    mouse = malloc(sizeof(Mouse));
    mouse->x = WINDOW_WIDTH / 2;
    mouse->y = WINDOW_HEIGHT / 2;
    mouse->first_move_handled = false;

    keyboard = malloc(sizeof(Keyboard));
    keyboard->w_pressed = false;
    keyboard->a_pressed = false;
    keyboard->s_pressed = false;
    keyboard->d_pressed = false;

    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game", NULL, NULL);

    if (!window)
    {
        printf("Failed to create GLFW window\n");
        exit(1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        exit(1);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, window_mouse_callback);
    glfwSetScrollCallback(window, window_scroll_callback);
}

int wireframe_key_state = GLFW_RELEASE;

void window_input()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        keyboard->w_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        keyboard->s_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        keyboard->a_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        keyboard->d_pressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        keyboard->w_pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        keyboard->s_pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
    {
        keyboard->a_pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
    {
        keyboard->d_pressed = false;
    }

    int new_wireframe_key_state = glfwGetKey(window, GLFW_KEY_U);
    if (new_wireframe_key_state == GLFW_RELEASE && wireframe_key_state == GLFW_PRESS)
    {
        renderer_toogle_wireframe();
    }
    wireframe_key_state = new_wireframe_key_state;
}
