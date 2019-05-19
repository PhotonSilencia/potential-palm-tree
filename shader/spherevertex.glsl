#version 330 core
in vec3 position;

uniform float radius;
uniform vec3 center;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 fragPos;

void main(){

    mat4 modelview = view * model;

    modelview[0][0] = 1.0;
    modelview[0][1] = 0.0;
    modelview[0][2] = 0.0;

    modelview[1][0] = 0.0;
    modelview[1][1] = 1.0;
    modelview[1][2] = 0.0;

    modelview[2][0] = 0.0;
    modelview[2][1] = 0.0;
    modelview[2][2] = 1.0;

    vec4 P = view * position;

    gl_Position = projection * P;

 /*   vec3 scaledPosition = position * radius;
    vec3 cameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraLeft_worldspace = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 vertexPosition_worldspace =
            center + cameraRight_worldspace * scaledPosition.x
            + cameraLeft_worldspace * scaledPosition.y;

    gl_Position = vec4(vertexPosition_worldspace, 1.0);

    fragPos = position;*/
}
