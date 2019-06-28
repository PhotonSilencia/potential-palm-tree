#version 330 core

in vec3 position;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main(void)
{
    gl_Position = projection * view * (0.03 * normalize(model * vec4(normal, 0.0)) + model * vec4(position, 1.0));
}
