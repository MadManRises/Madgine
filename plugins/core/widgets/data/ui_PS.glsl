#version 420 core


layout (std140, binding = 2) uniform PerObject
{
	bool hasDistanceField;
};

layout(binding = 0) uniform sampler2D tex;

in vec4 color;
in vec2 uv;

out vec4 fragColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}


void main()
{
	vec4 texColor;
	if (hasDistanceField){
		vec2 msdfUnit = 4.0/vec2(512.0, 512.0);
		vec4 sampled = texture2D(tex, uv);
		float sigDist = median(sampled.r, sampled.g, sampled.b) - 0.5;
		//sigDist *= dot(msdfUnit, vec2(0.5));
		sigDist *= 4.0;
		float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
		texColor = mix(vec4(0), vec4(1.0), opacity);
	} else {
		texColor = texture2D(tex, uv);
	}
    fragColor = color * texColor;
}