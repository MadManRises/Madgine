#version 100

precision mediump float;

//layout (location = 0) out vec4 FragColor;

uniform sampler2D tex;

varying vec4 color;
varying vec2 uv;

void main()
{
    gl_FragColor = color * texture2D(tex, uv);
}