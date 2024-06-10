#include "renderer.h"
#include "gfx.h"
#include "util.h"
#include "time.h"

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
    glGenBuffers(1, &renderer->instance_vbo_id);
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
}

void renderer_prepare()
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