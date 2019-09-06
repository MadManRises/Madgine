#version 100

precision mediump float;

//layout (location = 0) out vec4 FragColor;

varying vec4 color;

void main()
{
    gl_FragColor = color;
}