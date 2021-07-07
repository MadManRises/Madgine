
parameters Im3DPerApplication {
	float4x4(p);
};

parameters Im3DPerFrame {
	float4x4(v);
};

parameters Im3DPerObject {
	float4x4(m);
	
	bool(hasTexture);
	bool(hasDistanceField);
};