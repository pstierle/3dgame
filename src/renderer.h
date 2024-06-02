#pragma once

#include "defines.h"

void renderer_init();
void renderer_update();
void renderer_toogle_wireframe();
void renderer_vbo_data(GLsizeiptr size, void *data);
void renderer_vbo_sub_data(GLintptr offset, GLsizeiptr size, void *data);
void renderer_vbo_attr(GLuint index, GLint size);
void renderer_ibo_data(size_t size, void *data);
void renderer_ibo_sub_data(GLintptr offset, GLsizeiptr size, const void *data);
void renderer_ibo_draw(GLsizei count, void *indices);
void renderer_render();