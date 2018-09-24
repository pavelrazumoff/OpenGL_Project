#pragma once
#include "Mesh.h"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	Model() {}
	Model(char *path)
	{
		loadModel(path);
	}
	~Model();

	/* Functions */
	void loadModel(std::string path);
	void Draw(Shader shader);

private:
	/* Model Data */
	std::vector<Mesh*> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	/* Functions */
	void processNode(aiNode *node, const aiScene *scene);
	Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};