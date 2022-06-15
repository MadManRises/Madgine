#version 430 core

#include "sl_support.glsl"
#include "pointshadow.sl"

#include "light.glsl"


layout (std140, binding = 0) uniform PerApplication
{
	PointShadowPerApplication app;
};

layout (std140, binding = 1) uniform PerFrame
{
	PointShadowPerFrame frame;
};

layout (std140, binding = 2) uniform PerObject
{
	PointShadowPerObject object;
};

layout (std430, binding = 0) buffer Skeleton
{
	mat4 bones[];
};


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aPos2;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColor;
layout(location = 4) in vec2 aUV;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aWeights;
layout(location = 7) in mat4 aInstance_m;



void main()
{
	PointShadowInstanceData aInstance;
	aInstance.m = aInstance_m;

	vec4 worldPos;
	if (object.hasSkeleton){
		mat4 BoneTransform = bones[aBoneIDs[0]] * aWeights[0]
		+ bones[aBoneIDs[1]] * aWeights[1]
		+ bones[aBoneIDs[2]] * aWeights[2]
		+ bones[aBoneIDs[3]] * aWeights[3];
		worldPos = aInstance.m * BoneTransform * vec4(aPos, 1.0);
	}else{
		worldPos = aInstance.m * vec4(aPos, 1.0);
	}
    gl_Position = worldPos + vec4(aPos2, 0.0, 0.0) - vec4(frame.position, 0.0f);
	gl_Position.y *= -1;
}