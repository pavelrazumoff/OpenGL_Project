#pragma once
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	aiString path; // we store the path of the texture to compare with other textures.
};

struct Material {
	glm::vec4 diffuse_color;
	glm::vec4 specular_color;
	float shininess = 32;
};

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material);
	~Mesh();

	/* Mesh Data */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	Material material;

	/* Functions */
	void Draw(Shader shader, int numOfDrawCalls = 1);
	void setInstancesTransformAttributes();
	void setUseInstances(bool use);
private:
	/* Render data */
	unsigned int VAO, VBO, EBO;
	/* Functions */
	void setupMesh();
	bool useInstances = false;
};