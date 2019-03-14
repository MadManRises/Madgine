#version 300 es

precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;

out vec4 color;
out vec3 worldPos;
out vec3 normal;

uniform mat4 vp;
uniform mat4 m;

void main()
{
	worldPos = vec3(m * vec4(aPos, 1.0));
    gl_Position = vp * vec4(worldPos, 1.0);
    color = aColor;
	normal = mat3(transpose(inverse(m))) * aNormal;
}