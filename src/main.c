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

#define NUM_CUBES 10      // Change this to the desired number of cubes
#define CUBE_SPACING 1.0f // Change this to adjust spacing between cubes

extern GLuint program_id;
extern GLFWwindow *window;
extern Camera *camera;

int main()
{
    window_init();
    shader_init();
    renderer_init();
    camera_init();

    vec3 positions[] = {
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},
    };

    vec3 data[NUM_CUBES * 8];

    for (int i = 0; i < NUM_CUBES; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            glm_vec3_copy(positions[j], data[i * 8 + j]);
            data[i * 8 + j][0] += i * CUBE_SPACING;
        }
    }

    GLuint ibo_id;
    GLuint vbo_id;
    GLuint vao_id;

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

    glBufferData(GL_ARRAY_BUFFER, NUM_CUBES * 8 * 3 * sizeof(float), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    GLushort indices_first_cube[] = {
        0, 1, 2,
        1, 3, 4,
        5, 6, 3,
        7, 3, 6,
        2, 4, 7,
        0, 7, 6,
        0, 5, 1,
        1, 5, 3,
        5, 0, 6,
        7, 4, 3,
        2, 1, 4,
        0, 2, 7};

    GLushort indices[NUM_CUBES * 36];
    for (int i = 0; i < NUM_CUBES; i++)
    {
        for (int j = 0; j < 36; j++)
        {
            indices[i * 36 + j] = indices_first_cube[j] + i * 8;
        }
    }

    glGenBuffers(1, &ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint model_location = glGetUniformLocation(program_id, "model");
    GLuint view_location = glGetUniformLocation(program_id, "view");
    GLuint projection_location = glGetUniformLocation(program_id, "projection");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        renderer_update();
        window_input();
        camera_update();

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program_id);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        mat4 model = GLM_MAT4_IDENTITY_INIT;

        mat4 view = GLM_MAT4_IDENTITY_INIT;

        vec3 center;
        glm_vec3_add(camera->position, camera->front, center);

        glm_lookat(camera->position, center, camera->up, view);

        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        glm_perspective(glm_rad(camera->fov), 1600.0f / 900.0f, 0.1f, 100.0f, projection);

        glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat *)model);
        glUniformMatrix4fv(view_location, 1, GL_FALSE, (const GLfloat *)view);
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, (const GLfloat *)projection);

        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, NUM_CUBES * 36, GL_UNSIGNED_SHORT, 0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}