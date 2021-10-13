
#include "lightdata.sl"

float4 projectShadow(
    ShadowCaster caster,
    float4 pos)
{
    return mul(caster.viewProjectionMatrix, pos);
}

float4 castDirectionalLight(
    DirectionalLight light,
    float3 normal,
    float ambientFactor,
    float diffuseFactor)
{
    float3 ambient = ambientFactor * light.color;

    float3 norm = normalize(normal);
    float diff = max(dot(norm, -light.dir), 0.0);
    float3 diffuse = diffuseFactor * diff * light.color;

    return float4(ambient + diffuse, 1.0);
}

float4 castDirectionalShadowLight(
    DirectionalShadowLight light,
    float4 lightViewPosition,
    float3 normal,
    Texture2DMS<float> shadowMap,
    float ambientFactor,
    float diffuseFactor)
{
    float bias = 0.001;
    float3 normalizedLightViewPosition = (lightViewPosition.xyz / lightViewPosition.w) * 0.5 + 0.5;
    normalizedLightViewPosition.y = 1.0 - normalizedLightViewPosition.y;
    int2 lightTexCoord = int2(2048 * normalizedLightViewPosition.xy);

    float lightDepth = normalizedLightViewPosition.z - bias;

    float diffuseStrength = 1.0;

    for (int i = 0; i < light.caster.shadowSamples; ++i) {
        float shadowDepth = shadowMap.Load(lightTexCoord, i).r;
        diffuseStrength -= float(lightDepth > shadowDepth) / light.caster.shadowSamples;
    }

    return castDirectionalLight(light.light, normal, ambientFactor, diffuseFactor * diffuseStrength);
}

float4 castPointLight(
    PointLight light,
    float3 pos,
    float3 normal,
    float ambientFactor,
    float diffuseFactor)
{
    float3 ambient = ambientFactor * light.color;

    float3 lightDir = normalize(light.position - pos);
    float3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diffuseFactor * diff * light.color;

    float distance = length(light.position - pos);
    float attenuation = 1.0 / (light.constant + light.linearFactor * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;

    return float4(ambient + diffuse, 0.0);
}

float4 castPointShadowLight(
    PointLight light,
    float3 pos,
    float3 normal,
    TextureCube<float> shadowMap,
    SamplerState samplerState,
    float ambientFactor,
    float diffuseFactor)
{
     float bias = 0.001;
    float3 lightDir = pos - light.position;

    float lightDepth = length(lightDir) / 100.0 - bias;

    float ambientStrength = 1.0;
    float diffuseStrength = 1.0;
    
    float shadowDepth = shadowMap.Sample(samplerState, lightDir);
    diffuseStrength -= float(lightDepth > shadowDepth);
    ambientStrength -= float(lightDepth > shadowDepth);
    
    return castPointLight(light, pos, normal, ambientFactor * ambientStrength, diffuseFactor * diffuseStrength * 10.0);
}
