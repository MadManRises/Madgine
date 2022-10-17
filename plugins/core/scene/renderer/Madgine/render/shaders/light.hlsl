
#include "lightdata.sl"

float4 projectShadow(
    ShadowCaster caster,
    float4 pos)
{
    return mul(caster.viewProjectionMatrix, pos);
}

void castDirectionalLight(
    inout float4 diffuseIntensity,
    inout float4 specularIntensity,
    DirectionalLight light,
    float3 pos,
    float3 normal,
    float ambientFactor,
    float diffuseFactor,
    float specularFactor,
    float shininess)
{
    float3 ambient = ambientFactor * light.color;

    float3 norm = normalize(normal);
    
    float diff = max(dot(norm, -light.dir), 0.0);
    float3 diffuse = diffuseFactor * diff * light.color;

    diffuseIntensity += float4(ambient + diffuse, 1.0);

    float3 viewDir = normalize(/* -pos */ float3(0.0, 0.0, -1.0));
    float3 reflectDir = reflect(-light.dir, norm);
    float spec = pow(max(dot(viewDir, -reflectDir), 0.0), shininess);
    float3 specular = specularFactor * spec * light.color;

    specularIntensity += float4(specular, 1.0);
}

void castDirectionalShadowLight(
    inout float4 diffuseIntensity,
    inout float4 specularIntensity,
    DirectionalShadowLight light,
    float4 lightViewPosition,
    float3 pos,
    float3 normal,
    Texture2DMS<float> shadowMap,
    float ambientFactor,
    float diffuseFactor,
    float specularFactor,
    float shininess)
{
    float bias = 0.001;
    float3 normalizedLightViewPosition = (lightViewPosition.xyz / lightViewPosition.w) * 0.5 + 0.5;
    normalizedLightViewPosition.y = 1.0 - normalizedLightViewPosition.y;
    int2 lightTexCoord = int2(2048 * normalizedLightViewPosition.xy);

    float lightDepth = normalizedLightViewPosition.z - bias;

    float lightStrength = 1.0;

    for (int i = 0; i < light.caster.shadowSamples; ++i) {
        float shadowDepth = shadowMap.Load(lightTexCoord, i).r;
        lightStrength -= float(lightDepth > shadowDepth) / light.caster.shadowSamples;
    }

    castDirectionalLight(
        diffuseIntensity,
        specularIntensity, 
        light.light, 
        pos,
        normal, 
        ambientFactor, 
        diffuseFactor * lightStrength,
        specularFactor * lightStrength,
        shininess
    );
}

void castPointLight(
    inout float4 diffuseIntensity,
    inout float4 specularIntensity,
    PointLight light,
    float3 pos,
    float3 normal,
    float ambientFactor,
    float diffuseFactor,
    float specularFactor,
    float shininess)
{
    float3 ambient = ambientFactor * light.color;

    float3 lightDir = normalize(light.position - pos);
    float3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diffuseFactor * diff * light.color;

    float distance = length(light.position - pos);
    float attenuation = 1.0 / (light.constant + light.linearFactor * distance + light.quadratic * (distance * distance));

    diffuseIntensity += attenuation * float4(ambient + diffuse, 0.0);
}

void castPointShadowLight(
    inout float4 diffuseIntensity,
    inout float4 specularIntensity,
    PointLight light,
    float3 pos,
    float3 normal,
    TextureCube<float> shadowMap,
    SamplerState samplerState,
    float ambientFactor,
    float diffuseFactor,
    float specularFactor,
    float shininess)
{
    float bias = 0.001;
    float3 lightDir = pos - light.position;

    float lightDepth = length(lightDir) / 100.0 - bias;

    float lightStrength = 1.0;
    
    float shadowDepth = shadowMap.Sample(samplerState, lightDir);
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
