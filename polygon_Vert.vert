#version 450 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);

out vec3 normal;
void main()
{
    gl_Position = projection * view * model* vec4(vPos,1.0);
    normal = vNormal;
}
