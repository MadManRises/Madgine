

parameters ScenePerApplication {
	float4x4(p);
};

parameters ScenePerFrame {
	float4x4(v);

	float3(lightColor);
	float3(lightDir);
};

parameters ScenePerObject {

	float4x4(m);
	float4x4(anti_m);

	bool(hasLight);
	bool(hasTexture);
	bool(hasDistanceField);
	bool(hasSkeleton);

};