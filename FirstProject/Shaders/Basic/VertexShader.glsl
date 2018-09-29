#version 430 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0.
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

//out vec3 Normal;
//out vec3 FragPos;
//out vec2 TexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform bool useInstances;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
	// note that we read the multiplication from right to left
	if(!useInstances)
		gl_Position = projection * view * model * vec4(aPos, 1.0);
	else
		gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
}