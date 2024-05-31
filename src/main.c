#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "defines.h"
#include "shader.h"
#include "cglm/cglm.h"

extern GLuint program_id;

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    printf("%d \n", x_pos);
}

int main()
{
    GLFWwindow *window;

    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game", NULL, NULL);

    if (!window)
    {
        printf("Failed to create GLFW window\n");
        exit(1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        exit(1);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    init_shader();

    vec3 positions[] = {
        {0.0f, 1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f},
        {1.0f, -1.0f, 0.0f}};

    GLuint ibo_id;
    GLuint vbo_id;
    GLuint vao_id;

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

    glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    GLushort indices[] = {
        0, 1, 2};

    glGenBuffers(1, &ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint model_location = glGetUniformLocation(program_id, "model");
    GLuint view_location = glGetUniformLocation(program_id, "view");
    GLuint projection_location = glGetUniformLocation(program_id, "projection");

    vec3 cameraPos = {0.0f, 0.0f, 3.0f};
    vec3 cameraFront = {0.0f, 0.0f, -1.0f};
    vec3 cameraUp = {0.0f, 1.0f, 0.0f};

    const float camera_speed = 0.5f;
    float delta_time = 0.0f;
    float last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        float current_frame_time = glfwGetTime();
        delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            vec3 intermediate;
            glm_vec3_scale(cameraFront, camera_speed * delta_time, intermediate);
            glm_vec3_add(cameraPos, intermediate, cameraPos);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            vec3 intermediate;
            glm_vec3_scale(cameraFront, camera_speed * delta_time, intermediate);
            glm_vec3_sub(cameraPos, intermediate, cameraPos);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            vec3 intermediate;
            glm_vec3_cross(cameraFront, cameraUp, intermediate);
            glm_vec3_normalize(intermediate);
            glm_vec3_scale(intermediate, camera_speed * delta_time, intermediate);
            glm_vec3_add(cameraPos, intermediate, cameraPos);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            vec3 intermediate;
            glm_vec3_cross(cameraFront, cameraUp, intermediate);
            glm_vec3_normalize(intermediate);
            glm_vec3_scale(intermediate, camera_speed * delta_time, intermediate);
            glm_vec3_sub(cameraPos, intermediate, cameraPos);
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program_id);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        mat4 model = GLM_MAT4_IDENTITY_INIT;

        mat4 view = GLM_MAT4_IDENTITY_INIT;

        vec3 center;
        glm_vec3_add(cameraPos, cameraFront, center);

        glm_lookat(cameraPos, center, cameraUp, view);

        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        glm_perspective(glm_rad(45.0f), 1600.0f / 900.0f, 0.1f, 100.0f, projection);

        glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat *)model);
        glUniformMatrix4fv(view_location, 1, GL_FALSE, (const GLfloat *)view);
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, (const GLfloat *)projection);

        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}