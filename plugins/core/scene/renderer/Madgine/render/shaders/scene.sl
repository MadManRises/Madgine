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

	PointShadowLight pointLights[2];
};

struct SceneInstanceData{
	row_major float4x4 mv;
	row_major float4x4 anti_mv;
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