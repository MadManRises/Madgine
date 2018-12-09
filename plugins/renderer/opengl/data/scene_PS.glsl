#version 330 core

layout (location = 0) out vec4 FragColor;

in vec4 color;
in vec3 worldPos;
in vec3 normal;

uniform vec3 lightColor;
uniform vec3 lightDir;

void main()
{
    float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * lightColor;

	float diffuseStrength = 0.7;
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, -lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * diff * lightColor;

    FragColor = vec4(ambient + diffuse,1.0) * color;
}