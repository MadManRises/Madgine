#include "lightdata.sl"
#include "Madgine/render/shadinglanguage/memory.sl"

struct PointShadowPerApplication {
	float4x4 p;
};

struct PointShadowPerFrame {
	float3 position;
};

struct PointShadowInstanceData {
	float4x4 m;
	ArrayPtr<float4x4> bones;
};