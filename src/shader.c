#include <stdio.h>
#include <stdlib.h>
#include "shader.h"
#include "defines.h"

extern State state;

char *read_file(char path[])
{
    FILE *file;
    char *buffer = NULL;
    int buffer_size = 0;
    int i = 0;
    file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Error: Failed to open file '%s'.\n", path);
        exit(1);
    }

    int character;
    while ((character = fgetc(file)) != EOF)
    {
        if (i >= buffer_size)
        {
            buffer_size += 1000;
            buffer = realloc(buffer, buffer_size + 1); // +1 for the null terminator
            if (buffer == NULL)
            {
                printf("Error: while reading file.\n");
                fclose(file);
                exit(1);
            }
        }
        buffer[i] = character;
        i++;
    }

    // Null-terminate the buffer
    buffer[i] = '\0';

    fclose(file);

    return buffer;
}

void check_shader_status(GLuint shader_id)
{
    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

        GLchar buffer[log_length];

        GLsizei buffer_size;
        glGetShaderInfoLog(shader_id, log_length, &buffer_size, buffer);

        printf("Shader compile Error: %s", buffer);

        exit(1);
    }
}

void shader_init()
{
    Renderer *renderer = &state.renderer;

    char *fragment_shader = read_file("shaders/fragment_shader.glsl");
    char *vertex_shader = read_file("shaders/vertex_shader.glsl");

    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    const char *fragment_shader_source[] = {fragment_shader};
    const char *vertex_shader_source[] = {vertex_shader};

    glShaderSource(fragment_shader_id, 1, fragment_shader_source, NULL);
    glShaderSource(vertex_shader_id, 1, vertex_shader_source, NULL);

    glCompileShader(fragment_shader_id);
    glCompileShader(vertex_shader_id);

    check_shader_status(fragment_shader_id);
    check_shader_status(vertex_shader_id);

    renderer->program_id = glCreateProgram();

    glAttachShader(renderer->program_id, fragment_shader_id);
    glAttachShader(renderer->program_id, vertex_shader_id);

    glLinkProgram(renderer->program_id);

    glDeleteShader(fragment_shader_id);
    glDeleteShader(vertex_shader_id);

    free(fragment_shader);
    free(vertex_shader);
}