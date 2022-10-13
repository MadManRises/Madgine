#version 420 core

#include "sl_support.glsl"
#include "scene.sl"

#include "light.glsl"


layout (std140, binding = 0) uniform PerApplication
{
	ScenePerApplication app;
};

layout (std140, binding = 1) uniform PerFrame
{
	ScenePerFrame frame;
};

layout (std140, binding = 2) uniform PerObject
{
	ScenePerObject object;
};

layout(binding = 0) uniform sampler2D diffuseTex;
layout(binding = 1) uniform sampler2D emissiveTex;
layout(binding = 2) uniform sampler2DMS shadowDepthMap;
layout(binding = 3) uniform samplerCube pointShadowDepthMaps[2];


in vec4 color;
in vec4 worldPos;
in vec3 normal;
in vec2 uv;
in vec4 lightViewPosition;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;


float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}


void main()
{

	vec4 diffuseColor = color * object.diffuseColor;
	vec4 specularColor = object.specularColor;

	if (object.hasTexture){
		if (object.hasDistanceField){
			vec2 msdfUnit = 4.0/vec2(512.0, 512.0);
			vec4 sampled = texture2D(diffuseTex, uv);
			float sigDist = median(sampled.r, sampled.g, sampled.b) - 0.5;
			//sigDist *= dot(msdfUnit, vec2(0.5));
			sigDist *= 4.0;
			float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
			diffuseColor = mix(vec4(0), diffuseColor, opacity);
		}
		else
		{
			diffuseColor = diffuseColor * texture2D(diffuseTex, uv);
		}
	}

	if (object.hasLight){
		vec4 lightDiffuseIntensity = vec4(0);
		vec4 lightSpecularIntensity = vec4(0);

		castDirectionalShadowLight(
			lightDiffuseIntensity,
			lightSpecularIntensity,
			frame.light, 
			lightViewPosition, 
			normal,
			shadowDepthMap, 
			app.ambientFactor, 
			app.diffuseFactor,
			app.specularFactor,
			object.shininess
		);
		for (int i = 0; i < frame.pointLightCount; ++i){
			castPointShadowLight(
				lightDiffuseIntensity,
				lightSpecularIntensity,
				frame.pointLights[i],
				worldPos.xyz,
				normal,
				pointShadowDepthMaps[i],
				app.ambientFactor,
				app.diffuseFactor,
				app.specularFactor,
				object.shininess
			);
		}

		fragColor = 
			lightDiffuseIntensity * diffuseColor +
			lightSpecularIntensity * specularColor;
	}else{
		fragColor = diffuseColor;
	}

	if (app.hasHDR){
		brightColor += texture2D(emissiveTex, uv);
		brightColor.a = 1.0;
		float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1.0)
			brightColor = vec4(fragColor.rgb, 1.0);
	}else{
		fragColor.rgb += texture2D(emissiveTex, uv).rgb;
	}
}