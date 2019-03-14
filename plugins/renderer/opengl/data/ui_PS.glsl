#version 300 es

precision mediump float;

uniform sampler2D textures[2];

in vec4 color;
in vec2 uv;
flat in uint texIdx;

out vec4 FragColor;

void main()
{
   vec4 texColor = texture(textures[/*texIdx*/1], uv);
   FragColor = color * texColor;
}