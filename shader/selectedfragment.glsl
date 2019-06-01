#version 330 core

in vec3 fragNormal;
in vec3 lightDirection_cameraspace;
in vec3 eyeDirection_cameraspace;
in vec2 fragTexture;

uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float shine;
uniform float alpha;

uniform sampler2D textureSampler;

out vec4 finalColor;

void main(void)
{
    //Vektoren sind evtl. noch nicht korrekt
    //l = getBoundingBox().centre - position
    //v = Nullpunkt - position (im view space)
    vec3 n = normalize( fragNormal );
    vec3 l = normalize( lightDirection_cameraspace );
    vec3 r = normalize( reflect(l, n) );
    vec3 v = normalize( eyeDirection_cameraspace );

    //Phong Shading Calculation
    float cosTheta = clamp( dot( n, l), 0, 1);
    float cosPhi = clamp( dot( r, v), 0, 1);

    vec3 l_ambient = vec3(1.0, 1.0, 1.0) * matAmbient;
    vec3 l_diffuse = vec3(1.0, 1.0, 1.0) * matDiffuse * cosTheta;
    vec3 l_specular = vec3(1.0, 1.0, 1.0) * matSpecular * pow(cosPhi, shine);

    vec3 result = l_ambient + l_diffuse + l_specular;

    if (fragTexture.x > 0 && fragTexture.y > 0){
        vec3 color = texture( textureSampler, fragTexture ).rgb;
        result = result * color * 1.5;
    }

    finalColor = vec4(result, alpha);

}
