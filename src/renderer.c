#include "renderer.h"
#include "gfx.h"
#include "util.h"

extern State state;

void renderer_init()
{
    Renderer *renderer = &state.renderer;

    renderer->delta_time = 0.0f;
    renderer->last_frame_time = glfwGetTime();
    renderer->wireframe_enabled = false;

    renderer->model_location = glGetUniformLocation(renderer->program_id, "model");
    renderer->view_location = glGetUniformLocation(renderer->program_id, "view");
    renderer->projection_location = glGetUniformLocation(renderer->program_id, "projection");

    glGenVertexArrays(1, &renderer->vao_id);
    glGenBuffers(1, &renderer->vbo_id);
    glGenBuffers(1, &renderer->ibo_id);

    // TEST SCENE
    int num_cubes = 1000;
    int position_per_cube = 8;

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

    vec3 cube_block_positions[8000];

    int index = 0;

    for (int x = 0; x < 10; ++x)
    {
        for (int y = 0; y < 10; ++y)
        {
            for (int z = 0; z < 10; ++z)
            {
                for (int i = 0; i < position_per_cube; ++i)
                {
                    cube_block_positions[index][0] = cube_positions[i][0] + x;
                    cube_block_positions[index][1] = cube_positions[i][1] + y;
                    cube_block_positions[index][2] = cube_positions[i][2] + z;
                    index++;
                }
            }
        }
    }

    renderer_vbo_data(num_cubes * position_per_cube * 3 * sizeof(float), cube_block_positions);
    renderer_vbo_attr(0, 3);

    GLushort cube_indices[] = {
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

    GLushort cube_block_indices[36000];

    for (int i = 0; i < 1000; ++i)
    {
        for (int j = 0; j < 36; ++j)
        {
            cube_block_indices[i * 36 + j] = cube_indices[j] + i * 8;
        }
    }

    renderer_ibo_data(36000, cube_block_indices);
}

void renderer_vbo_data(GLsizeiptr size, void *data)
{
    glBindVertexArray(state.renderer.vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, state.renderer.vbo_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void renderer_vbo_attr(GLuint index, GLint size)
{
    glBindBuffer(GL_ARRAY_BUFFER, state.renderer.vbo_id);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(float), (void *)0);
}

void renderer_ibo_data(int num_elements, void *data)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.renderer.ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_elements * sizeof(GLushort), data, GL_STATIC_DRAW);

    Renderer *renderer = &state.renderer;
    renderer->ibo_num_indices = num_elements;
}

void renderer_ibo_draw(GLsizei count)
{
    glBindVertexArray(state.renderer.vbo_id);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
}

void renderer_update()
{
    Renderer *renderer = &state.renderer;
    Camera *camera = &state.camera;

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
    glUseProgram(state.renderer.program_id);

    if (state.renderer.wireframe_enabled == true)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    renderer_ibo_draw(state.renderer.ibo_num_indices);
}

void renderer_toogle_wireframe()
{
    Renderer *renderer = &state.renderer;

    if (renderer->wireframe_enabled == true)
    {
        renderer->wireframe_enabled = false;
    }
    else
    {
        renderer->wireframe_enabled = true;
    }
}