#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "defines.h"
#include "camera.h"
#include "shader.h"
#include "window.h"
#include "renderer.h"
#include "cglm/cglm.h"
#include "util.h"

extern GLFWwindow *window;

int main()
{
    window_init();
    shader_init();
    renderer_init();
    camera_init();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        window_input();
        camera_update();
        renderer_update();

        renderer_render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}