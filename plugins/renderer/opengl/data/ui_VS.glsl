#version 100
			
precision mediump float;

attribute vec3 aPos;
attribute vec4 aColor;
attribute vec2 aUV;

varying vec4 color;
varying vec2 uv;
//varying int texIdx;

void main()
{
   gl_Position = vec4((aPos * vec3(2,-2,-0.1)) - vec3(1,-1,0), 1.0);
   color = aColor;
   uv = aUV;
}