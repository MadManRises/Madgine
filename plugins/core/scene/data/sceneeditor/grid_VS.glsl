#version 420 core

layout (std140, binding = 1) uniform PerFrame
{
	mat4 vp;
};


layout(location = 0) in vec4 aPos;

out vec4 worldPos;

void main()
{	
	worldPos = aPos;
    gl_Position = vp * worldPos;
}