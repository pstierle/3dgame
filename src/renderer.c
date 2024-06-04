#include "renderer.h"
#include "gfx.h"
#include "util.h"
#define FNL_IMPL
#include "noise.h"
#include "time.h"

extern State state;

bool is_surrounded(vec3 position_data[], int index, int position_data_size)
{
    float pos_x = position_data[index][0];
    float pos_y = position_data[index][1];
    float pos_z = position_data[index][2];

    bool has_neighbor_xp = false, has_neighbor_xm = false;
    bool has_neighbor_yp = false, has_neighbor_ym = false;
    bool has_neighbor_zp = false, has_neighbor_zm = false;

    for (int i = 0; i < position_data_size; ++i)
    {
        if (has_neighbor_xp && has_neighbor_xm && has_neighbor_yp && has_neighbor_ym && has_neighbor_zp && has_neighbor_zm)
            return true;

        vec3 neighbor;
        glm_vec3_copy(position_data[i], neighbor);

        if (neighbor[0] == pos_x + 1 && neighbor[1] == pos_y && neighbor[2] == pos_z)
            has_neighbor_xp = true;
        else if (neighbor[0] == pos_x - 1 && neighbor[1] == pos_y && neighbor[2] == pos_z)
            has_neighbor_xm = true;
        else if (neighbor[0] == pos_x && neighbor[1] == pos_y + 1 && neighbor[2] == pos_z)
            has_neighbor_yp = true;
        else if (neighbor[0] == pos_x && neighbor[1] == pos_y - 1 && neighbor[2] == pos_z)
            has_neighbor_ym = true;
        else if (neighbor[0] == pos_x && neighbor[1] == pos_y && neighbor[2] == pos_z + 1)
            has_neighbor_zp = true;
        else if (neighbor[0] == pos_x && neighbor[1] == pos_y && neighbor[2] == pos_z - 1)
            has_neighbor_zm = true;
    }

    return has_neighbor_xp && has_neighbor_xm && has_neighbor_yp && has_neighbor_ym && has_neighbor_zp && has_neighbor_zm;
}

void generate_position_data(vec3 **position_data, int *position_data_size)
{
    int max_x = 40;
    int max_z = 40;

    *position_data = malloc(max_x * max_z * sizeof(vec3));
    *position_data_size = 0;

    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    noise.frequency = 0.01;
    noise.domain_warp_amp = 10.0f;
    noise.octaves = 6;
    noise.lacunarity = 1.5;

    for (int x = 0; x < max_x; ++x)
    {
        for (int z = 0; z < max_z; ++z)
        {
            float noise_val_y = fnlGetNoise3D(&noise, x, 0, z);
            float scaled_noise_val_y = fmaxf(0.0f, fminf(64.0f, noise_val_y * 20.0f + 32.0f));
            int rounded_noise_val_y = (int)roundf(scaled_noise_val_y);

            *position_data = realloc(**position_data, (*position_data_size + rounded_noise_val_y) * sizeof(vec3));

            for (int y = 0; y < rounded_noise_val_y; ++y)
            {
                (*position_data)[*position_data_size][0] = x;
                (*position_data)[*position_data_size][1] = y;
                (*position_data)[*position_data_size][2] = z;
                (*position_data_size)++;
            }
        }
    }
}

void filter_surrounded_cubes(vec3 **position_data, int *position_data_size)
{
    vec3 *filtered = malloc(*position_data_size * sizeof(vec3));
    int filtered_size = 0;

    for (int i = 0; i < *position_data_size; i++)
    {
        if (is_surrounded(*position_data, i, *position_data_size) == false)
        {
            glm_vec3_copy((*position_data)[i], filtered[filtered_size]);
            filtered_size++;
        }
    }

    filtered = realloc(filtered, filtered_size * sizeof(vec3));
    free(*position_data); // Free the original position data
    *position_data = filtered;
    *position_data_size = filtered_size;
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
    glGenBuffers(1, &renderer->instance_vbo_id);

    vec3 *position_data = NULL;
    int position_data_size = 0;

    clock_t start_time, end_time;
    double cpu_time_used;

    // Timing generate_position_data
    start_time = clock();
    generate_position_data(&position_data, &position_data_size);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("generate_position_data took %f seconds to execute \n", cpu_time_used);

    // Timing filter_surrounded_cubes
    start_time = clock();
    filter_surrounded_cubes(&position_data, &position_data_size);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("filter_surrounded_cubes took %f seconds to execute \n", cpu_time_used);

    vec3 cube_positions[] = {
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f}};

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

    glBindVertexArray(renderer->vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions), cube_positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->instance_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, position_data_size * sizeof(vec3), position_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    renderer->ibo_num_indices = sizeof(cube_indices) / sizeof(GLushort);
    renderer->num_instances = position_data_size;

    free(position_data);
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
    glm_perspective(glm_rad(camera->fov), 1600.0f / 900.0f, 0.1f, 50000.0f, projection);

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

    glBindVertexArray(state.renderer.vao_id);
    glDrawElementsInstanced(GL_TRIANGLES, state.renderer.ibo_num_indices, GL_UNSIGNED_SHORT, 0, state.renderer.num_instances);
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