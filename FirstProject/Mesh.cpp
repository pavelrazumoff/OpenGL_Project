#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = material;
	setupMesh();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader)
{
	unsigned int diffuseNr = 0;
	unsigned int specularNr = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding.
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(++diffuseNr); // transfer unsigned int to stream
		else if (name == "texture_specular")
			number = std::to_string(++specularNr); // transfer unsigned int to stream

		shader.setFloat(("material." + name + number).c_str(), i);
		//glUniform1i(glGetUniformLocation(shader.getShaderProgram(), (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	shader.setBool("material.use_texture_diffuse", diffuseNr > 0);
	shader.setBool("material.use_texture_specular", specularNr > 0);

	shader.setVec4("material.diffuse_color", material.diffuse_color);
	shader.setVec4("material.specular_color", material.specular_color);
	shader.setFloat("material.shininess", material.shininess);

	glActiveTexture(GL_TEXTURE0);
	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}