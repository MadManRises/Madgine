
#include "sl_support.glsl"
#include "lightdata.sl"


vec4 projectShadow(
	ShadowCaster caster, 
	vec4 pos
){
	return caster.viewProjectionMatrix * pos;
}

vec4 castDirectionalLight(
	DirectionalLight light, 
	vec3 normal, 
	float ambientFactor, 
	float diffuseFactor
){
	vec3 ambient = ambientFactor * light.color;
	
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, -light.dir), 0.0);
	vec3 diffuse = diffuseFactor * diff * light.color;

	return vec4(ambient + diffuse, 1.0);
}

vec4 castDirectionalShadowLight(
	DirectionalShadowLight light, 
	vec4 lightViewPosition, 
	vec3 normal, 
	sampler2DMS shadowMap, 
	float ambientFactor, 
	float diffuseFactor
){
	float bias = 0.001;
	vec3 normalizedLightViewPosition = (lightViewPosition.xyz / lightViewPosition.w) * 0.5 + 0.5;
	ivec2 lightTexCoord = ivec2(2048 * normalizedLightViewPosition.xy);
		
	float lightDepth = normalizedLightViewPosition.z - bias;

	float diffuseStrength = 1.0;

	for (int i = 0; i < light.caster.shadowSamples; ++i){
		float shadowDepth = texelFetch(shadowMap, lightTexCoord, i).r;	
		diffuseStrength -= float(lightDepth > shadowDepth) / light.caster.shadowSamples;
	}

	return castDirectionalLight(light.light, normal, ambientFactor, diffuseFactor * diffuseStrength);
}

vec4 castPointLight(
	PointLight light, 
	vec3 pos,
	vec3 normal, 
	float ambientFactor, 
	float diffuseFactor
){
	vec3 ambient = ambientFactor * light.color;
	
	vec3 lightDir = normalize(light.position - pos);
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseFactor * diff * light.color;

	float distance    = length(light.position - pos);
    float attenuation = 1.0 / (light.constant + light.linearFactor * distance + 
  			     light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;

	return vec4(ambient + diffuse, 0.0);
}

vec4 castPointShadowLight(
	PointLight light, 
	vec3 pos,
	vec3 normal, 
	samplerCube shadowMap, 
	float ambientFactor, 
	float diffuseFactor
){
	float bias = 0.001;
	vec3 lightDir = pos - light.position;
	
	float lightDepth = length(lightDir) / 100.0 - bias;

	float ambientStrength = 1.0;
	float diffuseStrength = 1.0;

	float shadowDepth = texture(shadowMap, lightDir).r;	
	diffuseStrength -= float(lightDepth > shadowDepth);
	ambientStrength -= float(lightDepth > shadowDepth);

	return castPointLight(light, pos, normal, ambientFactor * ambientStrength, diffuseFactor *  diffuseStrength * 10.0);
}
