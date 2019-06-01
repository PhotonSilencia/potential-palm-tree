#version 330 core

in vec3 position;
in vec3 normal;
in vec2 texCoords;
in vec3 lightPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPositions[9];

out vec2 fragTexture;
out vec3 fragNormal;
out vec3 lightDirection_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirections[9];

void main(void)
{
    gl_Position = projection * view * model * vec4(position, 1.0);

    vec3 vertexPosition_cameraspace = (view * model * vec4(position, 1.0)).xyz;
    eyeDirection_cameraspace = vec3(0.0, 0.0, 0.0) - vertexPosition_cameraspace;

    vec3 lightPosition_cameraspace = (view * vec4(lightPosition, 1.0)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace - eyeDirection_cameraspace;

//    lightDirections = vec3[9];

    for (int i = 0; i < 9; i++){
        vec3 currentLight = lightPositions[i];

        vec3 currentLight_cameraspace = (view * vec4(currentLight, 1.0)).xyz;
        lightDirections[i] = currentLight_cameraspace - vertexPosition_cameraspace;

    }

    //Normal of the vertex, in camera space
    mat4 mv_matrix = view * model;
    mat3 norm_matrix = transpose(inverse(mat3(mv_matrix)));
    fragNormal = norm_matrix * normal;

    fragTexture = texCoords;

}
