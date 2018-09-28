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
	void loadModel(std::string path, bool gamma = false);
	void setInstancesTransforms(glm::mat4* transforms, int size);
	void setUseInstances(bool use);
	void Draw(Shader shader, int numOfDrawCalls = 1);

private:
	/* Model Data */
	std::vector<Mesh*> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	bool gammaCorrection = false;
	/* Functions */
	void processNode(aiNode *node, const aiScene *scene);
	Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};