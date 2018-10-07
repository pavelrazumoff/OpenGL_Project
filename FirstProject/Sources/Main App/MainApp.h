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
	void initScene();
	void loadShaders();
	void loadTextures();
	void loadFonts();
	void initBuffers();
	void clearBuffers();
	void update();
	void render();
	void resize(int width, int height);

	void drawScene(Shader shader, bool finalDraw);
	void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	// input.
	void processInput(GLFWwindow *window);
	void processMouseMove(GLFWwindow* window, double xpos, double ypos);
	void processMouseClick(GLFWwindow* window, int button, int action, int mods);
	void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

	// buffers generation.
	void generateFontBuffers();

	// framebuffers generation.
	void generateFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useMultisampling, bool useHDR);
	void generateBloomFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useHDR);

	// framebuffers resize.
	void resizeFramebuffer(unsigned int FBO, unsigned int texBuffer, unsigned int RBO, bool useMultisampling, bool useHDR);
	void resizeBloomFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO, bool useHDR);
	void resizeGFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO);
	void resizeSsaoFramebuffer(unsigned int FBO, unsigned int texBuffer);

	GLFWwindow* getWindow();

	Shader basic_shader;
	Shader lamp_shader;
	Shader screen_shader;
	Shader skybox_shader;
	Shader simpleDepth_shader;
	Shader debugDepthQuad;
	Shader final_shader;
	Shader shaderBlur;

	//SSAO.
	Shader shaderGeometryPass;
	Shader shaderSSAO;
	Shader shaderSSAOBlur;

	//Font.
	Shader font_shader;

	Camera camera;
	Model model;
	Model planet;
	Model rock;

private:
	GLFWwindow* window = NULL;

	int screenWidth, screenHeight;
	int shadowWidth = 2048, shadowHeight = 2048;

	unsigned int framebuffer, intermediateFBO, finalFBO, depthMapFBO;
	unsigned int texColorMSBuffer, screenTexture, depthMap;
	unsigned int rbo, intermediateRBO, finalRBO;

	//SSAO.
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedo;
	unsigned int rboDepth;
	unsigned int ssaoFBO, ssaoBlurFBO;
	unsigned int noiseTexture;
	unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;

	unsigned int finalTextures[2];

	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];

	unsigned int VBO;
	unsigned int EBO;
	unsigned int lightVAO;
	unsigned int quadVAO, quadVBO;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int floorVAO;

	GLuint fontVAO, fontVBO;

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
	bool useAmbientOcclusion = true;

	glm::vec3 lightPos;
	glm::vec3 lightColors[4];

	// Fonts.
	std::map<GLchar, Character> Characters;
};