
#version 450 core

const vec4 skyColor = vec4(0.9, 0.9, 1.0, 1.0);
const vec4 groundColor = vec4(0.3, 0.3, 0.2, 1.0);

in vec3 normal;
out vec4 fragColor;
void main()
{

    vec3 normalized_normal = normalize(normal);
    float w = 0.5 * (1.0 + dot(normalized_normal, vec3(0.0, 1.0, 0.0)));
    vec4 diffuseColor = w * skyColor + (1.0 - w) * groundColor;
    fragColor = diffuseColor;
    //fragColor = vec4(1,0,0,1);
}
