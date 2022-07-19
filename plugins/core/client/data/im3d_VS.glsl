#version 430 core

#include "sl_support.glsl"
#include "im3d.sl"

layout (std140, binding = 0) uniform PerApplication
{
	Im3DPerApplication app;
};

layout (std140, binding = 1) uniform PerFrame
{
	Im3DPerFrame frame;
};

layout (std140, binding = 2) uniform PerObject
{
	Im3DPerObject object;
};


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aPos2;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColor;
layout(location = 4) in vec2 aUV;

out vec4 color;
out vec4 worldPos;
out vec3 normal;
out vec2 uv;


void main()
{
	worldPos = object.m * vec4(aPos, 1.0);
	
    gl_Position = app.p * (frame.v * worldPos + vec4(aPos2, 0.0, 0.0));	
	gl_Position.y *= -1;
    color = aColor;
	normal = aNormal;
	uv = vec2(aUV.x, aUV.y);	
}