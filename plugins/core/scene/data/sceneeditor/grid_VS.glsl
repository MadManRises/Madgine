#version 100

precision mediump float;

attribute vec4 aPos;

varying vec4 worldPos;

uniform mat4 mvp;

void main()
{	
	worldPos = aPos;
    gl_Position = mvp * worldPos;
	gl_Position.z *= -1.0;
}