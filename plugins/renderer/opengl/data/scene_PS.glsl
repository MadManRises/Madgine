#version 100

precision mediump float;

//layout (location = 0) out vec4 FragColor;

varying vec4 color;
varying vec3 worldPos;
varying vec3 normal;

uniform vec3 lightColor;
uniform vec3 lightDir;

void main()
{
    float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * lightColor;

	float diffuseStrength = 0.7;
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, -lightDir), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

    gl_FragColor = vec4(ambient + diffuse,1.0) * color;
}