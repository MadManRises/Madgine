#include "lightdata.sl"
#include "Madgine/render/shadinglanguage/memory.sl"

struct PointShadowPerApplication {
	float4x4 p;
};

struct PointShadowInstanceData {
	float4x4 mv;
	//ArrayPtr<float4x4> bones;
};