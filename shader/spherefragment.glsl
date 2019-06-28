#version 330 core

in vec3 fragmentCameraSpace;
in vec3 m_center;

uniform float radius;

uniform mat4 view, projection;
uniform vec3 cameraPosition;

uniform samplerCube textureCube;

out vec4 finalColor;

void main(void)
{

    vec3 centerCameraSpace = (view * vec4(m_center, 1.0)).xyz;

    vec3 rayDirection = normalize(fragmentCameraSpace);
    vec3 rayOrigin = vec3(0.0);

    float a = dot(rayDirection, rayDirection);
    // b = 2v(P0-O)
    float b = 2.0 * dot(rayDirection, rayOrigin - centerCameraSpace);
    // c = C² - r² / (P0-O)²-r²
    float c = dot(rayOrigin - centerCameraSpace, rayOrigin - centerCameraSpace) - (radius * radius);
    float d = b * b - 4.0 * a * c;

    if (d >= 0){

        // t = -2vC +- sqrt((2vC²- 4 * C² - r²)) / (2 * a)
        float t1 = (-b + sqrt(d)) / (2.0 * a);
        float t2 = (-b - sqrt(d)) / (2.0 * a);
        float t = t1 < 0 ? (t2 < 0 ? 0 : t2) : (t2 < 0 ? t1: min(t1, t2));

        //P = tV + E, E = vec3(0.0) in camera space
        vec3 pCameraSpace = t * rayDirection;

        // p in projection
        vec4 pClipSpace = projection * vec4(pCameraSpace, 1.0);

        float zNDC = pClipSpace.z / pClipSpace.w;
        float zWindowSpace = (gl_DepthRange.far - gl_DepthRange.near) / 2 * zNDC
                              + (gl_DepthRange.far + gl_DepthRange.near) / 2;

        gl_FragDepth = zWindowSpace;

        // Switch to world space for refraction
        vec3 pWorldSpace = (inverse(view) * vec4(pCameraSpace, 1.0)).xyz;
        vec3 enterNormal = normalize(pWorldSpace - m_center);
        vec3 cameraToEnterNormal = normalize(pWorldSpace - cameraPosition);

        // Refraction
        float refraction = 1.5;
        vec3 refractionDirection = refract(cameraToEnterNormal, enterNormal, 1.0 / refraction);

        vec3 refractionRay = refractionDirection;

        // Wechsel zurück in Camera Space ist unnötig und funktioniert nicht richtig
        //vec3 refractionCameraSpace = (view * vec4(refractionDirection, 1.0)).xyz;

        // exit Refraction
        // a = 1
        // Camera Space: Refraction funktioniert, aber Zoomproblem
        //b = 2.0 * dot(refractionCameraSpace, pCameraSpace - centerCameraSpace);
        //c = dot(pCameraSpace - centerCameraSpace, pCameraSpace - centerCameraSpace) - (radius * radius);


        // Alternativ im World Space, kein Zoomproblem, Normale invertiert
        b = 2.0 * dot(refractionDirection, pWorldSpace - m_center);
        c = dot(pWorldSpace - m_center, pWorldSpace - m_center) - radius * radius;

        d = b * b - 4.0 * c;

        if (d >= 0){

            t1 = (-b + sqrt(d)) / 2.0;

            t = t1;

            vec3 exitPWorldSpace = pWorldSpace + t * refractionDirection;

            //Normale wird invertiert um korrekte Refraktion zu erhalten. Siehe
            //https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
            vec3 exitNormal = -normalize(exitPWorldSpace - m_center);
            vec3 cameraToExitNormal = normalize(exitPWorldSpace - cameraPosition);

            vec3 exitRefractionRay = refract(cameraToExitNormal, exitNormal, refraction / 1.0);

            refractionRay = exitRefractionRay;

        } else {
            discard;
        }

        finalColor = texture(textureCube, refractionRay);

    } else {
        discard;
    }
}
