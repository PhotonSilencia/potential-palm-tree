#version 330 core

in vec3 fragmentCameraSpace;

uniform float radius;
uniform mat4 view, projection;
uniform vec3 cameraPosition;

uniform samplerCube textureCube;

out vec4 finalColor;

void main(void)
{

    vec3 rayDirection = normalize(fragmentCameraSpace);
    vec3 rayOrigin = vec3(0.0);

    float a = dot(rayDirection, rayDirection);
    float b = 2 * dot(rayDirection, rayOrigin);
    float c = dot( rayOrigin, rayOrigin ) - radius * radius;
    float d = b * b - 4 * a * c;

    if (d > -0.0000001){
        float t1 = (-b + sqrt(d)) / (2 * a);
        float t2 = (-b - sqrt(d)) / (2 * a);
        float t = t1 < 0 ? (t2 < 0 ? 0 : t2) : (t2 < 0 ? t1: min(t1, t2));

        //P = P0 + tV
        vec3 pCameraSpace = t * rayDirection;

        vec4 pClipSpace = projection * vec4(pCameraSpace, 1.0);

        // puts billboard in front
        float zNDC = pClipSpace.z / pClipSpace.w;

        float zWindowSpace = (gl_DepthRange.far - gl_DepthRange.near) / 2*zNDC
                + (gl_DepthRange.far + gl_DepthRange.near) / 2;

        gl_FragDepth = zWindowSpace;

        // Reflection
        vec3 pWorldSpace = (inverse(view) * vec4( pCameraSpace, 1.0)).xyz;
        vec3 normal = normalize(pWorldSpace);
        vec3 cameraNormal = normalize (pWorldSpace - cameraPosition);

        vec3 refractionRay = refract(cameraNormal, normal, 1.0 / 1.2);

        b = 2 * dot(refractionRay, pWorldSpace);
        c = dot( pWorldSpace, pWorldSpace) - radius * radius;
        d = b * b - 4 * c;

        if(d > -0.0000001){
            t1 = (-b + sqrt(d)) / 2;
            t2 = (-b - sqrt(d)) / (2 * a);
            t = t1 < 0 ? (t2 < 0 ? 0 : t2) : (t2 < 0 ? t1: min(t1, t2));

            vec3 exitP = pWorldSpace + t * refractionRay;

            vec3 exitNormal = normalize(exitP);
            vec3 exitCameraNormal = normalize(exitP - cameraPosition);
            vec3 exitRefractionRay = refract(exitCameraNormal, exitNormal, 1.2 / 1.0);

            refractionRay = exitRefractionRay;
        }

        finalColor = texture(textureCube, refractionRay);

    } else {
        discard;
    }
}
