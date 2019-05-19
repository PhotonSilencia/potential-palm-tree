#version 330 core
in vec3 skybox_texcoords;

uniform samplerCube skybox;

out vec4 FragColor;

void main()
{
    FragColor = texture(skybox, skybox_texcoords);
}
