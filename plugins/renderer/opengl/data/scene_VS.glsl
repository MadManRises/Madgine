#version 100

precision mediump float;

attribute vec3 aPos;
attribute vec4 aColor;
attribute vec3 aNormal;

varying vec4 color;
varying vec3 worldPos;
varying vec3 normal;

uniform mat4 vp;
uniform mat4 m;

void main()
{
	worldPos = vec3(m * vec4(aPos, 1.0));
    gl_Position = vp * vec4(worldPos, 1.0);
	gl_Position.z *= -1.0f;
    color = aColor;
	normal = mat3(transpose(inverse(m))) * aNormal;
}