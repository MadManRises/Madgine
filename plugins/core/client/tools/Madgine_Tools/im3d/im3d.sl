
parameters Im3DPerApplication {
	float4x4(p);
};

parameters Im3DPerFrame {
	float4x4(v);

	float3(lightColor);
	float3(lightDir);
};

parameters Im3DPerObject {

	float4x4(m);
	float4x4(anti_m);

	bool(hasLight);
	bool(hasTexture);
	bool(hasDistanceField);

};