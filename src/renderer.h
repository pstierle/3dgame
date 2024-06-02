#pragma once

#include "defines.h"

void renderer_init();
void renderer_update();
void renderer_toogle_wireframe();
void renderer_vbo_data(GLsizeiptr size, void *data);
void renderer_vbo_attr(GLuint index, GLint size);
void renderer_ibo_data(int num_elements, void *data);
void renderer_ibo_draw(GLsizei count);
void renderer_render();