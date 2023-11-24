#include "glfw3.h"
#ifndef State_H
#define State_H

struct State
{
    GLFWwindow *window;
    float deltaTime;
    unsigned int shaderProgram;
    unsigned int VAO;
    unsigned int VBO;
};

#endif