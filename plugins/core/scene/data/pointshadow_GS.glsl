#version 430 core

#include "sl_support.glsl"
#include "pointshadow.sl"

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

out vec4 FragPos;

layout (std140, binding = 0) uniform PerApplication
{
	PointShadowPerApplication app;
};


void helper(mat4 m, int layer){
    gl_Layer = layer; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) // for each triangle vertex
    {
        FragPos = gl_in[i].gl_Position;
        gl_Position = app.p * m * gl_in[i].gl_Position;
        EmitVertex();
    }    
    EndPrimitive();
}

void main()
{
   mat4 mx1 = mat4(vec4(0,0,-1,0), vec4(0,1,0,0), vec4(1,0,0,0), vec4(0,0,0,1));
   mat4 mx2 = mat4(vec4(0,0,1,0), vec4(0,1,0,0), vec4(-1,0,0,0), vec4(0,0,0,1));
   mat4 my1 = mat4(vec4(1,0,0,0), vec4(0,0,-1,0), vec4(0,1,0,0), vec4(0,0,0,1));
   mat4 my2 = mat4(vec4(1,0,0,0), vec4(0,0,1,0), vec4(0,-1,0,0), vec4(0,0,0,1));
   mat4 mz1 = mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1));
   mat4 mz2 = mat4(vec4(-1,0,0,0), vec4(0,1,0,0), vec4(0,0,-1,0), vec4(0,0,0,1));
   
   helper(mx1, 0);
   helper(mx2, 1);
   helper(my1, 2);
   helper(my2, 3);
   helper(mz1, 4);
   helper(mz2, 5);
}  