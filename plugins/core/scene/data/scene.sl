#include "lightdata.sl"

struct ScenePerApplication {
	float4x4 p;

	bool hasHDR;

	float ambientFactor;
	float diffuseFactor;
	float specularFactor;
};

struct ScenePerFrame {
	int pointLightCount;

	DirectionalShadowLight light;

	PointLight pointLights[2];
};

struct SceneInstanceData{
	float4x4 mv;
	float4x4 anti_mv;
};

struct ScenePerObject {

	float4 diffuseColor;
	float4 specularColor;

	float shininess;

	bool hasLight;
	bool hasTexture;
	bool hasDistanceField;
	bool hasSkeleton;

};