#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform sampler2D ssaoTexture;

uniform bool gammaCorrection;
uniform bool useHDR;
uniform bool useBloom;
uniform bool useAmbientOcclusion;
uniform float exposure;

const float offset = 1.0 / 500.0;

void main()
{
	vec3 result = texture(screenTexture, TexCoords).rgb;
	float gamma = 2.2;

	if (useBloom)
	{
		vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;
		result += bloomColor; // additive blending.
	}

	if (useHDR)
	{
		// reinhard
		// vec3 result = result / (result + vec3(1.0));
		// exposure
		result = vec3(1.0) - exp(-result * exposure);
	}

	if (gammaCorrection)
		result = pow(result.rgb, vec3(1.0 / gamma));

	if (useAmbientOcclusion)
	{
		float ambientOcclusion = texture(ssaoTexture, TexCoords).r;
		result *= ambientOcclusion;
	}

	FragColor = vec4(result, 1.0);
}