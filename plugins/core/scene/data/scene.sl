

parameters ScenePerApplication {
	float4x4(p);
	float4x4(lightProjectionMatrix);
};

parameters ScenePerFrame {
	float4x4(v);
	float4x4(lightViewMatrix);

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