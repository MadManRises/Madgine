#ifndef _LIGHDATA_SL_
#define _LIGHDATA_SL_

struct DirectionalLight {
	float3 color;
	float3 dir;
};

struct PointLight {
	float3 position;
	float3 color;

	float constant;
	float linearFactor;
	float quadratic;
};

struct ShadowCaster {
	float4x4 reprojectionMatrix;

	int shadowSamples;
};

struct DirectionalShadowLight {
	DirectionalLight light;

	ShadowCaster caster;
};

struct PointShadowLight {
	PointLight light;

	ShadowCaster caster;
};

#endif