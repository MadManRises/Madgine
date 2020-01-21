#version 420 core

layout (std140, binding = 0) uniform PerApplication
{
	mat4 p;	
};

layout (std140, binding = 1) uniform PerFrame
{
	mat4 v;

	vec3 lightColor;
	vec3 lightDir;
};

layout (std140, binding = 2) uniform PerObject
{
	mat4 m;
	mat4 anti_m;

	bool hasLight;
	bool hasTexture;
	bool hasDistanceField;
};


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aPos2;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec2 aUV;

out vec4 color;
out vec3 worldPos;
out vec3 normal;
out vec2 uv;


void main()
{
	worldPos = vec3(m * vec4(aPos, 1.0));
    gl_Position = p * (v * vec4(worldPos, 1.0) + vec4(aPos2, 0.0, 0.0));	
    color = aColor;
	normal = mat3(anti_m) * aNormal;
	uv = aUV;
}