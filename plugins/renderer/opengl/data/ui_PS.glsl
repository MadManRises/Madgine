#version 100

precision mediump float;

uniform sampler2D textures[2];

varying vec4 color;
varying vec2 uv;
//varying int texIdx;

//out vec4 FragColor;

void main()
{
   vec4 texColor = texture2D(textures[/*texIdx*/1], uv);
   gl_FragColor = color * texColor;
}