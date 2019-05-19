#version 330 core
in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 skybox_texcoords;

void main()
{
    skybox_texcoords = position;
    mat4 view_without_translate = mat4(mat3(view));
    gl_Position = projection * view_without_translate * vec4(position, 1.0);
}
