#version 430 core

layout(location = 0) in vec3 aPos; // the position variable has attribute position 0.
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform bool useInstances;

void main()
{
	mat4 worldMat;
	if (!useInstances)
		worldMat = model;
	else
		worldMat = aInstanceMatrix;

	vec4 viewPos = view * worldMat * vec4(aPos, 1.0);

	FragPos = viewPos.xyz;
	TexCoords = aTexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(view * worldMat)));
	Normal = normalMatrix * aNormal;

	gl_Position = projection * viewPos;
}