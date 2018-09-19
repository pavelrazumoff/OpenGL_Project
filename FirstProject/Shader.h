#pragma once

#include "Header.h"

class Shader
{
public:
	// constructor reads and builds the shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);

	void load(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

	unsigned int getShaderProgram();

private:
	unsigned int ShaderProgram;
};