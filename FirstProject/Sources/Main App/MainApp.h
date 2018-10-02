#pragma once

#include "../Shaders/Shader.h"
#include "../Miscellaneous/Camera.h"
#include "../Models/Model.h"
#include "MainData.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class MainApp
{
public:
	MainApp() {}
	MainApp(int screenWidth, int screenHeight);

	void setScreenSize(int screenWidth, int screenHeight);

	int initWindow();
	void init();
	void loadShaders();
	void loadTextures();
	void initBuffers();
	void clearBuffers();
	void update();
	void render();
	void resize(int width, int height);

	void drawScene(Shader shader);

	// input.
	void processInput(GLFWwindow *window);
	void processMouseMove(GLFWwindow* window, double xpos, double ypos);
	void processMouseClick(GLFWwindow* window, int button, int action, int mods);
	void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

	void generateFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useMultisampling, bool useHDR);
	void generateBloomFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useHDR);
	void resizeFramebuffer(unsigned int FBO, unsigned int texBuffer, unsigned int RBO, bool useMultisampling, bool useHDR);
	void resizeBloomFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO, bool useHDR);

	GLFWwindow* getWindow();

	Shader basic_shader;
	Shader lamp_shader;
	Shader screen_shader;
	Shader skybox_shader;
	Shader simpleDepth_shader;
	Shader debugDepthQuad;
	Shader final_shader;
	Shader shaderBlur;

	Camera camera;
	Model model;
	Model planet;
	Model rock;

private:
	GLFWwindow* window = NULL;

	int screenWidth, screenHeight;
	int shadowWidth = 2048, shadowHeight = 2048;

	unsigned int VBO;
	unsigned int EBO;
	unsigned int framebuffer, intermediateFBO, finalFBO, depthMapFBO;
	unsigned int texColorMSBuffer, screenTexture, depthMap;
	unsigned int rbo, intermediateRBO, finalRBO;

	unsigned int finalTextures[2];

	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];

	unsigned int lightVAO;
	unsigned int quadVAO, quadVBO;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int floorVAO;

	unsigned int diffuseMap;
	unsigned int specularMap;
	unsigned int cubemapTexture;
	unsigned int woodTexture;

	unsigned int uboMatrices;

	unsigned int amount = 10000;
	glm::mat4* modelMatrices = NULL;

	float deltaTime = 0.0f; // Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float lastMouseX = 400, lastMouseY = 300;
	bool firstMouseUse = true;
	bool lbutton_down = false;

	bool useMultisampling = true;
	bool useGammaCorrection = true;
	bool useShadowMapping = false;
	bool useHDR = true;
	bool useBloom = true;
};