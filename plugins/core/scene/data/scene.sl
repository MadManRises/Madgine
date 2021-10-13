#include "lightdata.sl"

struct ScenePerApplication {
	float4x4 p;

	bool hasHDR;

	float ambientFactor;
	float diffuseFactor;
};

struct ScenePerFrame {
	int pointLightCount;
	float4x4 v;

	DirectionalShadowLight light;

	PointLight pointLights[2];
};

struct SceneInstanceData{
	float4x4 m;
	float4x4 anti_m;
};

struct ScenePerObject {

	float4 diffuseColor;

	bool hasLight;
	bool hasTexture;
	bool hasDistanceField;
	bool hasSkeleton;

};