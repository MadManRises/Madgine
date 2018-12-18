#version 330 core

uniform sampler2D textures[2];

in vec4 color;
in vec2 uv;
flat in uint texIdx;

out vec4 FragColor;

void main()
{
   vec4 texColor = texture2D(textures[/*texIdx*/1], uv);
   FragColor = color * texColor;
}