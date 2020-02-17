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
	bool hasSkeleton;
};

layout (std140, binding = 3) uniform Skeleton
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

out vec4 color;
out vec3 worldPos;
out vec3 normal;
out vec2 uv;


void main()
{
	if (hasSkeleton){
		mat4 BoneTransform = bones[aBoneIDs[0]] * aWeights[0];
		BoneTransform += bones[aBoneIDs[1]] * aWeights[1];
		BoneTransform += bones[aBoneIDs[2]] * aWeights[2];
		BoneTransform += bones[aBoneIDs[3]] * aWeights[3];
		worldPos = vec3(m * BoneTransform * vec4(aPos, 1.0));
	}else{
		worldPos = vec3(m * vec4(aPos, 1.0));
	}
    gl_Position = p * (v * vec4(worldPos, 1.0) + vec4(aPos2, 0.0, 0.0));	
    color = aColor;
	normal = mat3(anti_m) * aNormal;
	uv = vec2(aUV.x, 1.0 - aUV.y);
}