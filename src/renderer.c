#include "renderer.h"
#include "gfx.h"
#include "util.h"
#define FNL_IMPL
#include "noise.h"
#include "time.h"

extern State state;

bool is_surrounded(vec3 position_data[], int index, int position_data_size)
{
    vec3 pos = {position_data[index][0], position_data[index][1], position_data[index][2]};

    bool has_neighbor_xp = false, has_neighbor_xm = false;
    bool has_neighbor_yp = false, has_neighbor_ym = false;
    bool has_neighbor_zp = false, has_neighbor_zm = false;

    for (int i = 0; i < position_data_size; ++i)
    {
        if (i == index)
            continue;
        vec3 neighbor = {position_data[i][0], position_data[i][1], position_data[i][2]};

        if (neighbor[0] == pos[0] + 1 && neighbor[1] == pos[1] && neighbor[2] == pos[2])
            has_neighbor_xp = true;
        if (neighbor[0] == pos[0] - 1 && neighbor[1] == pos[1] && neighbor[2] == pos[2])
            has_neighbor_xm = true;
        if (neighbor[0] == pos[0] && neighbor[1] == pos[1] + 1 && neighbor[2] == pos[2])
            has_neighbor_yp = true;
        if (neighbor[0] == pos[0] && neighbor[1] == pos[1] - 1 && neighbor[2] == pos[2])
            has_neighbor_ym = true;
        if (neighbor[0] == pos[0] && neighbor[1] == pos[1] && neighbor[2] == pos[2] + 1)
            has_neighbor_zp = true;
        if (neighbor[0] == pos[0] && neighbor[1] == pos[1] && neighbor[2] == pos[2] - 1)
            has_neighbor_zm = true;
    }

    return has_neighbor_xp && has_neighbor_xm && has_neighbor_yp && has_neighbor_ym && has_neighbor_zp && has_neighbor_zm;
}

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

    int max_x = 200;
    int max_y = 1;
    int max_z = 200;

    vec3 *position_data = malloc(max_x * max_y * max_z * sizeof(vec3));
    int position_data_size = 0;
    int extra_added = 0;

    fnl_state noise = fnlCreateState();

    for (int x = 0; x < max_x; ++x)
    {
        for (int y = 0; y < max_y; ++y)
        {
            for (int z = 0; z < max_z; ++z)
            {
                float noise_val_y = fnlGetNoise3D(&noise, x, y, z);
                float scaled_noise_val_y = fmaxf(0.0f, fminf(30.0f, noise_val_y * 20.0f + 10.0f));
                float rounded_noise_val_y = roundf(scaled_noise_val_y);

                position_data[position_data_size][0] = x;
                position_data[position_data_size][1] = y + rounded_noise_val_y;
                position_data[position_data_size][2] = z;
                position_data_size++;

                /*                 if (rounded_noise_val_y > 0)
                                {
                                    int diff = (int)rounded_noise_val_y;
                                    extra_added += diff;
                                    position_data = realloc(position_data, (max_x * max_y * max_z * sizeof(vec3)) + (extra_added * sizeof(vec3)));

                                    while (rounded_noise_val_y > 0.0f)
                                    {
                                        rounded_noise_val_y -= 1.0f;
                                        position_data[position_data_size][0] = x;
                                        position_data[position_data_size][1] = y + rounded_noise_val_y;
                                        position_data[position_data_size][2] = z;
                                        position_data_size++;
                                    }
                                } */
            }
        }
    }

    vec3 cube_positions[8] = {
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},
    };

    vec3 *cubes_vertex_data = malloc(position_data_size * 8 * sizeof(vec3));

    for (int i = 0; i < position_data_size; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            vec3 intermediate;
            glm_vec3_add(position_data[i], cube_positions[j], intermediate);

            cubes_vertex_data[i * 8 + j][0] = intermediate[0];
            cubes_vertex_data[i * 8 + j][1] = intermediate[1];
            cubes_vertex_data[i * 8 + j][2] = intermediate[2];
        }
    }

    renderer_vbo_data(position_data_size * 8 * sizeof(vec3), cubes_vertex_data);
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

    GLushort *cube_block_indices = malloc(sizeof(GLushort) * position_data_size * 36);

    for (int i = 0; i < position_data_size; ++i)
    {
        for (int j = 0; j < 36; ++j)
        {
            cube_block_indices[i * 36 + j] = cube_indices[j] + i * 8;
        }
    }

    renderer_ibo_data(position_data_size * 36 * sizeof(GLushort), cube_block_indices);
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