#define GLFW_INCLUDE_NONE

#include "renderer.h"
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "util.h"

Renderer *renderer;
extern GLuint program_id;
extern Camera *camera;

#define NUM_CUBES 10
#define NUM_CUBE_POSITION_ELEMENTS 3
#define NUM_CUBE_POSITIONS 8
#define CUBE_POSITION_ELEMENT_SIZE sizeof(float)
#define CUBE_SIZE NUM_CUBE_POSITIONS *NUM_CUBE_POSITION_ELEMENTS *CUBE_POSITION_ELEMENT_SIZE

void test_scene_init()
{
    // Initialize VBO data with enough space for all cubes
    renderer_vbo_data(CUBE_SIZE * NUM_CUBES, NULL);

    // Define the positions of the vertices of a single cube
    vec3 cube_positions[] = {
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},
    };

    for (int i = 0; i < NUM_CUBES; i++) // Note the change in the loop index starting from 0
    {
        vec3 cube[8];
        for (int j = 0; j < 8; j++)
        {
            glm_vec3_copy(cube_positions[j], cube[j]);
            cube[j][0] += i * 1.0f; // Apply the offset to the x value
        }
        size_t offset = i * CUBE_SIZE; // Adjust the offset calculation
        renderer_vbo_sub_data(offset, CUBE_SIZE, cube);
    }

    renderer_vbo_attr(0, 3);

    GLushort indices_cube[] = {
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

    // Initialize IBO data with enough space for all cubes
    renderer_ibo_data(36 * sizeof(GLushort) * NUM_CUBES, NULL);

    for (int i = 0; i < NUM_CUBES; i++) // Note the change in the loop index starting from 0
    {
        size_t offset = i * 36 * sizeof(GLushort); // Adjust the offset calculation
        renderer_ibo_sub_data(offset, 36 * sizeof(GLushort), indices_cube);
    }
}

void test_scence_render()
{
    for (int i = 0; i < NUM_CUBES; i++) // Note the change in the loop index starting from 0
    {
        size_t offset = i * 36 * sizeof(GLushort); // Adjust the offset calculation
        renderer_ibo_draw(36, (void *)offset);
    }
}

void renderer_init()
{
    renderer = malloc(sizeof(Renderer));
    renderer->delta_time = 0.0f;
    renderer->last_frame_time = glfwGetTime();
    renderer->wireframe_enabled = false;

    renderer->model_location = glGetUniformLocation(program_id, "model");
    renderer->view_location = glGetUniformLocation(program_id, "view");
    renderer->projection_location = glGetUniformLocation(program_id, "projection");

    glGenVertexArrays(1, &renderer->vao_id);
    glGenBuffers(1, &renderer->vbo_id);
    glGenBuffers(1, &renderer->ibo_id);

    test_scene_init();
}

void renderer_vbo_data(GLsizeiptr size, void *data)
{
    glBindVertexArray(renderer->vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void renderer_vbo_sub_data(GLintptr offset, GLsizeiptr size, void *data)
{
    glBindVertexArray(renderer->vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void renderer_vbo_attr(GLuint index, GLint size)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_id);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(float), (void *)0);
}

void renderer_ibo_data(size_t size, void *data)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void renderer_ibo_sub_data(GLintptr offset, GLsizeiptr size, const void *data)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo_id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void renderer_ibo_draw(GLsizei count, void *indices)
{
    glBindVertexArray(renderer->vao_id);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, indices);
}

void renderer_update()
{
    float current_frame_time = glfwGetTime();
    renderer->delta_time = current_frame_time - renderer->last_frame_time;
    renderer->last_frame_time = current_frame_time;

    mat4 model = GLM_MAT4_IDENTITY_INIT;

    mat4 view = GLM_MAT4_IDENTITY_INIT;

    vec3 center;
    glm_vec3_add(camera->position, camera->front, center);

    glm_lookat(camera->position, center, camera->up, view);

    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_perspective(glm_rad(camera->fov), 1600.0f / 900.0f, 0.1f, 100.0f, projection);

    glUniformMatrix4fv(renderer->model_location, 1, GL_FALSE, (const GLfloat *)model);
    glUniformMatrix4fv(renderer->view_location, 1, GL_FALSE, (const GLfloat *)view);
    glUniformMatrix4fv(renderer->projection_location, 1, GL_FALSE, (const GLfloat *)projection);
}

void renderer_render()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program_id);

    if (renderer->wireframe_enabled == true)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    test_scence_render();
}

void renderer_toogle_wireframe()
{
    if (renderer->wireframe_enabled == true)
    {
        renderer->wireframe_enabled = false;
    }
    else
    {
        renderer->wireframe_enabled = true;
    }
    print_s("Wireframe Enabled Changed: ");
    print_i(renderer->wireframe_enabled);
}