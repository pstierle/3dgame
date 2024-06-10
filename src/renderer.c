#include "renderer.h"
#include "gfx.h"
#include "util.h"
#define FNL_IMPL
#include "noise.h"
#include "time.h"

Chunk *chunks;
int chunks_count = 100;

extern State state;

void generate_chunks()
{
    int chunk_size = 200;

    chunks = malloc(sizeof(Chunk) * chunks_count);

    fnl_state noise = fnlCreateState();
    noise.seed = 1337;
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    noise.frequency = 0.005f;
    noise.octaves = 5;
    noise.lacunarity = 2.0f;
    noise.gain = 0.5f;
    noise.fractal_type = FNL_FRACTAL_FBM;

    for (int i = 0; i < chunks_count; i++)
    {
        Chunk *chunk = malloc(sizeof(Chunk));
        chunk->data = malloc(chunk_size * chunk_size * sizeof(vec3) * 100);

        int offset_x = (i % (int)sqrt(chunks_count)) * chunk_size;
        int offset_z = (i / (int)sqrt(chunks_count)) * chunk_size;

        chunk->position[0] = offset_x + chunk_size / 2;
        chunk->position[1] = 0.0f;
        chunk->position[2] = offset_z + chunk_size / 2;

        int current_chunk_size = 0;

        for (int x = 0; x < chunk_size; ++x)
        {
            for (int z = 0; z < chunk_size; ++z)
            {
                float noise_val_y = fnlGetNoise3D(&noise, x + offset_x, 0, z + offset_z);
                float scaled_noise_val_y = fmaxf(0.0f, fminf(128.0f, noise_val_y * 60.0f + 64.0f));
                int rounded_noise_val_y = (int)roundf(scaled_noise_val_y);

                chunk->data[current_chunk_size][0] = x + offset_x;
                chunk->data[current_chunk_size][1] = rounded_noise_val_y;
                chunk->data[current_chunk_size][2] = z + offset_z;

                current_chunk_size++;
            }
        }

        chunk->cube_count = current_chunk_size;
        chunks[i] = *chunk;
    }
}

void load_chunk(int index)
{
    Renderer *renderer = &state.renderer;

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
    glBufferData(GL_ARRAY_BUFFER, chunks[index].cube_count * sizeof(vec3), chunks[index].data, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    renderer->ibo_num_indices = sizeof(cube_indices) / sizeof(GLushort);
    renderer->num_instances = chunks[index].cube_count;
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

    generate_chunks();
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
    glm_perspective(glm_rad(camera->fov), 1600.0f / 900.0f, 0.1f, 5000.0f, projection);

    glUniformMatrix4fv(renderer->model_location, 1, GL_FALSE, (const GLfloat *)model);
    glUniformMatrix4fv(renderer->view_location, 1, GL_FALSE, (const GLfloat *)view);
    glUniformMatrix4fv(renderer->projection_location, 1, GL_FALSE, (const GLfloat *)projection);

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

    for (int i = 0; i < chunks_count; i++)
    {
        if (glm_vec3_distance(camera->position, chunks[i].position) < 500.0f)
        {
            load_chunk(i);
            glBindVertexArray(state.renderer.vao_id);
            glDrawElementsInstanced(GL_TRIANGLES, state.renderer.ibo_num_indices, GL_UNSIGNED_SHORT, 0, state.renderer.num_instances);
        }
    }
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