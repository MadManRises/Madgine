#version 100

precision mediump float;

//layout (location = 0) out vec4 FragColor;

varying vec4 color;
varying vec3 worldPos;
varying vec3 normal;
varying vec2 uv;

uniform vec3 lightColor;
uniform vec3 lightDir;

uniform bool hasLight;
uniform bool hasTexture;

uniform sampler2D tex;

void main()
{

	vec4 colorAcc = color;

	if (hasLight){
		float ambientStrength = 0.4;
		vec3 ambient = ambientStrength * lightColor;

		float diffuseStrength = 0.7;
		vec3 norm = normalize(normal);
		float diff = max(dot(norm, -lightDir), 0.0);
		vec3 diffuse = diffuseStrength * diff * lightColor;

		colorAcc = vec4(ambient + diffuse,1.0) * colorAcc;
	}

	if (hasTexture){
		colorAcc = colorAcc * texture2D(tex, uv);
	}

    gl_FragColor = colorAcc;
}