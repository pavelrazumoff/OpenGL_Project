#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform bool gammaCorrection;
uniform bool useHDR;
uniform float exposure;

const float offset = 1.0 / 500.0;

void main()
{
	vec3 result = texture(screenTexture, TexCoords).rgb;
	float gamma = 2.2;

	if (useHDR)
	{
		// reinhard
		// vec3 result = result / (result + vec3(1.0));
		// exposure
		result = vec3(1.0) - exp(-result * exposure);
	}

	if (gammaCorrection)
		result = pow(result.rgb, vec3(1.0 / gamma));
	FragColor = vec4(result, 1.0);
}