
#include "sl_support.glsl"
#include "lightdata.sl"


vec4 projectShadow(
	ShadowCaster caster, 
	vec4 pos
){
	return caster.viewProjectionMatrix * pos;
}

void castDirectionalLight(
	inout vec4 diffuseIntensity,
    inout vec4 specularIntensity,
	DirectionalLight light, 
	vec3 normal, 
	float ambientFactor, 
	float diffuseFactor,
    float specularFactor,
    float shininess
){
	vec3 ambient = ambientFactor * light.color;
	
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, -light.dir), 0.0);
	vec3 diffuse = diffuseFactor * diff * light.color;

	diffuseIntensity += vec4(ambient + diffuse, 1.0);
}

void castDirectionalShadowLight(
	inout vec4 diffuseIntensity,
    inout vec4 specularIntensity,
	DirectionalShadowLight light, 
	vec4 lightViewPosition, 
	vec3 normal, 
	sampler2DMS shadowMap, 
	float ambientFactor, 
	float diffuseFactor,
    float specularFactor,
    float shininess
){
	float bias = 0.001;
	vec3 normalizedLightViewPosition = (lightViewPosition.xyz / lightViewPosition.w) * 0.5 + 0.5;
	ivec2 lightTexCoord = ivec2(2048 * normalizedLightViewPosition.xy);
		
	float lightDepth = normalizedLightViewPosition.z - bias;

	float lightStrength = 1.0;

	for (int i = 0; i < light.caster.shadowSamples; ++i){
		float shadowDepth = texelFetch(shadowMap, lightTexCoord, i).r;	
		lightStrength -= float(lightDepth > shadowDepth) / light.caster.shadowSamples;
	}

	castDirectionalLight(
		diffuseIntensity,
        specularIntensity, 
		light.light, 
		normal, 
		ambientFactor, 
		diffuseFactor * lightStrength,
		specularFactor * lightStrength,
		shininess
	);
}

void castPointLight(
	inout vec4 diffuseIntensity,
    inout vec4 specularIntensity,
	PointLight light, 
	vec3 pos,
	vec3 normal, 
	float ambientFactor, 
	float diffuseFactor,
    float specularFactor,
    float shininess
){
	vec3 ambient = ambientFactor * light.color;
	
	vec3 lightDir = normalize(light.position - pos);
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseFactor * diff * light.color;

	float distance = length(light.position - pos);
    float attenuation = 1.0 / (light.constant + light.linearFactor * distance + light.quadratic * (distance * distance));

	diffuseIntensity += attenuation * vec4(ambient + diffuse, 0.0);
}

void castPointShadowLight(
	inout vec4 diffuseIntensity,
    inout vec4 specularIntensity,
	PointLight light, 
	vec3 pos,
	vec3 normal, 
	samplerCube shadowMap, 
	float ambientFactor, 
	float diffuseFactor,
    float specularFactor,
    float shininess
){
	float bias = 0.001;
	vec3 lightDir = pos - light.position;
	
	float lightDepth = length(lightDir) / 100.0 - bias;

	float lightStrength = 1.0;

	float shadowDepth = texture(shadowMap, lightDir).r;	
	lightStrength -= float(lightDepth > shadowDepth);

	castPointLight(
		diffuseIntensity,
        specularIntensity, 
		light, 
		pos, 
		normal, 
		ambientFactor, 
		diffuseFactor * lightStrength * 10.0,
		specularFactor * lightStrength * 10.0,
		shininess
	);
}
