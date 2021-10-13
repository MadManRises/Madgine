#version 420 core

#include "sl_support.glsl"
#include "blur.sl"

layout (std140, binding = 0) uniform Data
{
	BlurData data;
};

layout(binding = 0) uniform sampler2D input;

in vec2 texCoord;

out vec4 fragColor;
  
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{		
    vec2 tex_offset = 1.0 / data.textureSize; // gets size of single texel
    vec3 result = texture(input, texCoord).rgb * weight[0]; // current fragment's contribution
    if(data.horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(input, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(input, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(input, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(input, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    fragColor = vec4(result, 1.0);
}