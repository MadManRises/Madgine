#version 430 core

#include "sl_support.glsl"
#include "scene.sl"

#include "light.glsl"


layout (std140, binding = 0) uniform PerApplication
{
	ScenePerApplication app;
};

layout (std140, binding = 1) uniform PerFrame
{
	ScenePerFrame frame;
};

layout (std140, binding = 2) uniform PerObject
{
	ScenePerObject object;
};

layout (std430, binding = 0) buffer Skeleton
{
	mat4 bones[];
};


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aPos2;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec2 aUV;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aWeights;
layout(location = 7) in SceneInstanceData aInstance;

out vec4 color;
out vec4 worldPos;
out vec3 normal;
out vec2 uv;
out vec4 lightViewPosition;




void main()
{
	if (object.hasSkeleton){
		mat4 BoneTransform = bones[aBoneIDs[0]] * aWeights[0]
		+ bones[aBoneIDs[1]] * aWeights[1]
		+ bones[aBoneIDs[2]] * aWeights[2]
		+ bones[aBoneIDs[3]] * aWeights[3];
		worldPos = aInstance.m * BoneTransform * vec4(aPos, 1.0);
	}else{
		worldPos = aInstance.m * vec4(aPos, 1.0);
	}
    gl_Position = app.p * (frame.v * worldPos + vec4(aPos2, 0.0, 0.0));	
	gl_Position.y *= -1;

    color = aColor;
	normal = mat3(aInstance.anti_m) * aNormal;
	uv = vec2(aUV.x, aUV.y);
	lightViewPosition = projectShadow(frame.light.caster, worldPos);
}