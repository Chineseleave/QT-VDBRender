
#version 450 core
layout (location = 0) in vec3 Pos;
uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);
void main()
{
    gl_Position = projection * view * model * vec4(Pos,1.0);
}
