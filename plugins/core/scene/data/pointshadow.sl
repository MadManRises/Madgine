#include "lightdata.sl"

struct PointShadowPerApplication {
	float4x4 p;
};

struct PointShadowPerFrame {
	float3 position;
};

struct PointShadowInstanceData {
	float4x4 m;
};

struct PointShadowPerObject {
	bool hasSkeleton;
};