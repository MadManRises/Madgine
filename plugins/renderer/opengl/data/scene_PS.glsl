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
uniform bool hasDistanceField;

uniform sampler2D tex;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{

	vec4 colorAcc = color;

	if (hasTexture){
		if (hasDistanceField){
			vec2 msdfUnit = 4.0/vec2(512.0, 512.0);
			vec4 sample = texture2D(tex, uv);
			float sigDist = median(sample.r, sample.g, sample.b) - 0.5;
			//sigDist *= dot(msdfUnit, vec2(0.5));
			sigDist *= 4.0;
			float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
			colorAcc = mix(vec4(0,0,0,0), colorAcc, opacity);
		}
		else
		{
			colorAcc = colorAcc * texture2D(tex, uv);
		}
	}

	if (hasLight){
		float ambientStrength = 0.4;
		vec3 ambient = ambientStrength * lightColor;

		float diffuseStrength = 0.7;
		vec3 norm = normalize(normal);
		float diff = max(dot(norm, -lightDir), 0.0);
		vec3 diffuse = diffuseStrength * diff * lightColor;

		colorAcc = vec4(ambient + diffuse,1.0) * colorAcc;
	}

    gl_FragColor = colorAcc;
}