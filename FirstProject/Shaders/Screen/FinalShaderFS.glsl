#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec3 result = texture(screenTexture, TexCoords).rgb;
	FragColor = vec4(result, 1.0);
	float brightness = dot(FragColor.rgb, vec3(0.7126, 0.7152, 0.7722));
	if (brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
}