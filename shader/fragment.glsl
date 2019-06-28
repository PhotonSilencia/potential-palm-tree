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

uniform int lightCount;
in vec3 lightDirections[9];

uniform vec3 lightAmbient[9];
uniform vec3 lightDiffuse[9];
uniform vec3 lightSpecular[9];

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
    cosPhi = pow(cosPhi, shine);

    //Cel Shading
    float celDiffuse, celSpecular;
    if (cosTheta <= 0.33){
        celDiffuse = 0.0;
    } else if (cosTheta <= 0.7){
        celDiffuse = 0.5;
    } else {
        celDiffuse = 1.0;
    }

    if (cosPhi <= 0.33){
        celSpecular = 0.0;
    } else if (cosPhi <= 0.7){
        celSpecular = 0.5;
    } else {
        celSpecular = 1.0;
    }

    vec3 l_ambient = vec3(1.0, 1.0, 1.0) * matAmbient;
    vec3 l_diffuse = vec3(1.0, 1.0, 1.0) * matDiffuse * celDiffuse;
    vec3 l_specular = vec3(1.0, 1.0, 1.0) * matSpecular * celSpecular;

    vec3 result = l_ambient + l_diffuse + l_specular;

    for (int i = 0; i < 9; i++){

        if (i >= lightCount){
            break;
        }

        vec3 currentLightDirection = normalize ( lightDirections[i] );
        vec3 currentLightReflect = normalize ( reflect (currentLightDirection, n));

        float currentLightCosTheta = clamp( dot( n, currentLightDirection), 0, 1);
        float currentLightCosPhi = clamp( dot( currentLightReflect, v), 0, 1);

        vec3 currentLightAmbient = lightAmbient[i] * matAmbient;
        vec3 currentLightDiffuse = lightDiffuse[i] * matDiffuse * currentLightCosTheta;
        vec3 currentLightSpecular = lightSpecular[i] * matSpecular * pow(currentLightCosPhi, shine);

        result += currentLightAmbient + currentLightDiffuse + currentLightSpecular;
    }

    if (fragTexture.x > 0 && fragTexture.y > 0){
        vec3 color = texture( textureSampler, fragTexture ).rgb;
        result = result * color;
    }

    finalColor = vec4(result, alpha);

}
