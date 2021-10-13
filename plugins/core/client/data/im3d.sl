
struct Im3DPerApplication {
	float4x4 p;
};

struct Im3DPerFrame {
	float4x4 v;
};

struct Im3DPerObject {
	float4x4 m;
	
	bool hasTexture;
	bool hasDistanceField;
};