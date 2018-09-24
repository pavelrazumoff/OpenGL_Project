#pragma once
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	aiString path; // we store the path of the texture to compare with other textures.
};

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();

	/* Mesh Data */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	/* Functions */
	void Draw(Shader shader);
private:
	/* Render data */
	unsigned int VAO, VBO, EBO;
	/* Functions */
	void setupMesh();
};