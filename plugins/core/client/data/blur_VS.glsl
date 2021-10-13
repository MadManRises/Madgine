#version 420 core

layout(location = 0) in vec3 aPos;

out vec2 texCoord;

void main()
{		
    texCoord = (aPos.xy + vec2(1.0, 1.0)) * 0.5;
    gl_Position = vec4(aPos, 1.0);
}