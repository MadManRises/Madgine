#version 100

precision mediump float;

attribute vec3 aPos;
attribute vec2 aPos2;
attribute vec4 aColor;
attribute vec3 aNormal;
attribute vec2 aUV;

varying vec4 color;
varying vec3 worldPos;
varying vec3 normal;
varying vec2 uv;

uniform mat4 v;
uniform mat4 p;
uniform mat4 m;
uniform mat3 anti_m;

void main()
{
	worldPos = vec3(m * vec4(aPos, 1.0));
    gl_Position = p * (v * vec4(worldPos, 1.0) + vec4(aPos2, 0.0, 0.0));
	gl_Position.z *= -1.0;
    color = aColor;
	normal = anti_m * aNormal;
	uv = vec2(aUV.x, 1.0 - aUV.y);
}