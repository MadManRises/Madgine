#version 300 es
			
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;
layout (location = 3) in uint textureIdx;

out vec4 color;
out vec2 uv;
flat out uint texIdx;

void main()
{
   gl_Position = vec4((aPos * vec3(2,-2,-0.1)) - vec3(1,-1,0), 1.0);
   color = aColor;
   uv = vec2(aUV.x, 1.0 - aUV.y);
   texIdx = textureIdx;
}