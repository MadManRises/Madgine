#version 420 core

#include "sl_support.glsl"
#include "im3d.sl"

layout (std140, binding = 0) uniform PerApplication
{
	Im3DPerApplication app;
};

layout (std140, binding = 1) uniform PerFrame
{
	Im3DPerFrame frame;
};

layout (std140, binding = 2) uniform PerObject
{
	Im3DPerObject object;
};

layout(binding = 0) uniform sampler2D tex;


in vec4 color;
in vec4 worldPos;
in vec3 normal;
in vec2 uv;

out vec4 fragColor;


float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{

	vec4 colorAcc = color;

	if (object.hasTexture){
		if (object.hasDistanceField){
			vec2 msdfUnit = 4.0/vec2(512.0, 512.0);
			vec4 sampled = texture2D(tex, uv);
			float sigDist = median(sampled.r, sampled.g, sampled.b) - 0.5;
			//sigDist *= dot(msdfUnit, vec2(0.5));
			sigDist *= 4.0;
			float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
			colorAcc = mix(vec4(0), colorAcc, opacity);
		}
		else
		{
			colorAcc = colorAcc * texture2D(tex, uv);
		}
	}

    fragColor = colorAcc;
}