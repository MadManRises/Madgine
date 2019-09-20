#version 100

precision mediump float;

uniform sampler2D texture;

varying vec4 color;
varying vec2 uv;

//out vec4 FragColor;

void main()
{
   vec4 texColor = texture2D(texture, uv);
   gl_FragColor = color * texColor;
}