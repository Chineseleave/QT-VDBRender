
#version 450 core
layout (location = 0) in vec3 position;

uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);
uniform mat4 VdbBoxSize = mat4(1.0);//vdb的缩放矩阵

out vec3 mBBoxmin;
out vec3 mBBoxmax;

out vec3 vPosition;
vec3 vdbBoxSize;
//smooth out vec3 vUV; //3D texture coordinates for texture lookup in the fragment shader
void main()
{
    gl_Position = projection * view * model * VdbBoxSize *vec4(position,1);
    vPosition = vec3(VdbBoxSize*vec4(position,1));
    mBBoxmin = vec3(-VdbBoxSize[0][0]/2, -VdbBoxSize[1][1]/2, -VdbBoxSize[2][2]/2);
    mBBoxmax = vec3(VdbBoxSize[0][0]/2, VdbBoxSize[1][1]/2, VdbBoxSize[2][2]/2);
}
