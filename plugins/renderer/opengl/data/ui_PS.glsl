#version 100

precision mediump float;

uniform sampler2D texture;

varying vec4 color;
varying vec2 uv;

uniform bool hasDistanceField;

//out vec4 FragColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}


void main()
{
	vec4 texColor;
	if (hasDistanceField){
		vec2 msdfUnit = 4.0/vec2(512.0, 512.0);
		vec4 sample = texture2D(texture, uv);
		float sigDist = median(sample.r, sample.g, sample.b) - 0.5;
		//sigDist *= dot(msdfUnit, vec2(0.5));
		sigDist *= 4.0;
		float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
		texColor = mix(vec4(0,0,0,0), vec4(1.0), opacity);
	} else {
		texColor = texture2D(texture, uv);
	}
    gl_FragColor = color * texColor;
}