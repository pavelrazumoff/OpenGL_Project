#include "MainApp.h"

MainApp::MainApp(int screenWidth, int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

int MainApp::initWindow()
{
	//glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_click_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void MainApp::setScreenSize(int screenWidth, int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

GLFWwindow* MainApp::getWindow()
{
	return window;
}

void MainApp::init()
{
	camera.init(glm::vec3(0.0f, 1.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	//uncomment it if not using models loading.
	//stbi_set_flip_vertically_on_load(true);

	loadShaders();
	loadTextures();
	initBuffers();

	model.loadModel("Models//house_01.obj", useGammaCorrection);
	planet.loadModel("Models//planet.obj", useGammaCorrection);
	rock.loadModel("Models//rock.obj", useGammaCorrection);

	rock.setInstancesTransforms(modelMatrices, amount);

	initScene();
}

void MainApp::update()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void MainApp::resize(int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	resizeFramebuffer(framebuffer, texColorMSBuffer, rbo, useMultisampling, useHDR);
	resizeFramebuffer(intermediateFBO, screenTexture, intermediateRBO, false, useHDR);
	resizeBloomFramebuffer(finalFBO, finalTextures, finalRBO, useHDR);
	for (int i = 0; i < 2; ++i)
		resizeFramebuffer(pingpongFBO[i], pingpongColorbuffers[i], -1, false, useHDR);

	unsigned int gTextures[] = { gPosition, gNormal, gAlbedo };
	resizeGFramebuffer(gBuffer, gTextures, rboDepth);

	resizeSsaoFramebuffer(ssaoFBO, ssaoColorBuffer);
	resizeSsaoFramebuffer(ssaoBlurFBO, ssaoColorBufferBlur);
}

void MainApp::clearBuffers()
{
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteFramebuffers(1, &ssaoFBO);
	glDeleteFramebuffers(1, &ssaoBlurFBO);

	for (int i = 0; i < 2; ++i)
		glDeleteFramebuffers(1, &pingpongFBO[i]);

	glDeleteFramebuffers(1, &finalFBO);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteFramebuffers(1, &intermediateFBO);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &skyboxVBO);

	delete modelMatrices;
	modelMatrices = NULL;
}