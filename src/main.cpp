#include <glad/glad.h>
#include "glfw3.h"
#include <iostream>
#include "state.hpp"
#include "game.hpp"

extern State state;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    state.window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

    if (state.window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(state.window);
    glfwSetFramebufferSizeCallback(state.window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    Game game;

    bool setup = game.setup();

    if (!setup)
    {
        return -1;
    }

    while (!glfwWindowShouldClose(state.window))
    {
        game.handleInput();
        game.update();
        game.render();

        glfwPollEvents();
        glfwSwapBuffers(state.window);
    }

    glfwTerminate();

    return 0;
}
