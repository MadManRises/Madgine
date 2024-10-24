#version 330 core
in vec4 FragPos;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / 100.0;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  