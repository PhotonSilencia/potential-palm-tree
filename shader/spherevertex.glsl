#version 330 core
in vec3 position;

out vec3 fragmentCameraSpace;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 center;

void main(){

   vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
   vec3 up = vec3(view[0][1], view[1][1], view[2][1]);

   vec4 fragmentViewSpace =
        view * vec4(position.x * right + position.y * up, 1.0);

    gl_Position = projection * fragmentViewSpace;

    fragmentCameraSpace = fragmentViewSpace.xyz;

}
