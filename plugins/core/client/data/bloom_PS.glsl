#version 420 core

#include "bloom.sl"

layout (std140, binding = 0) uniform Data
{
	BloomData data;
};

layout(binding = 0) uniform sampler2D input;
layout(binding = 1) uniform sampler2D blurred;

in vec2 texCoord;

out vec4 fragColor;
  

void main()
{		
    const float gamma = 2.2;
    vec3 hdrColor = texture(input, texCoord).rgb;      
    vec3 bloomColor = texture(blurred, texCoord).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * data.exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
}