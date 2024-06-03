#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 instanceOffset; 

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 model = mat4(1.0); 
    model[3] = vec4(instanceOffset, 1.0); 

    gl_Position = projection * view * model * vec4(position, 1.0);
}