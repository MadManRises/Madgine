#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec4 aColor;
layout(location = 4) in vec2 aUV;

out vec4 color;
out vec2 uv;
//out int texIdx;

void main()
{
   gl_Position = vec4((aPos * vec3(2,-2,0)) - vec3(1,-1,0), 1.0);
   gl_Position.z = 1.0 / (1.0 + aPos.z);
   color = aColor;
   uv = vec2(aUV.x, aUV.y);
}