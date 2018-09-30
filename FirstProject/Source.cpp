#include "Shader.h"
#include "Camera.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void init();
void loadShaders();
void loadTextures();
void initBuffers();
void clearBuffers();
void update();
void render();

void drawScene(Shader shader);

void generateFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useMultisampling, bool useHDR);
void generateBloomFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useHDR);
void resizeFramebuffer(unsigned int FBO, unsigned int texBuffer, unsigned int RBO, bool useMultisampling, bool useHDR);
void resizeBloomFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO, bool useHDR);

int screenWidth, screenHeight;
int shadowWidth = 2048, shadowHeight = 2048;

float vertices[] = {
	// Back face
	-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
	 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	 0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
	 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
	-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
	// Front face
	-0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
	 0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
	 0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, // top-right
	 0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // top-left
	-0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
	// Left face
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	// Right face
	 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	 0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
	 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	 0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
	// Bottom face
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
	 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	-0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	// Top face
	-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	 0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
	 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	-0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
};

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

unsigned int indices[] = { // note that we start from 0!
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};

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

unsigned int amount = 10000;
glm::mat4* modelMatrices = NULL;

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f, 0.2f, 2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f, 2.0f, -12.0f),
	glm::vec3(0.0f, 0.0f, -3.0f)
};

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

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Uncomment this if uses Mac OS X.
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	screenWidth = 1300;
	screenHeight = 760;

	//glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
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
	
	init();

	while (!glfwWindowShouldClose(window))
	{
		// Input.
		processInput(window);

		// Updating.
		update();

		// Rendering.
		render();

		// Swap buffers and check and call events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	clearBuffers();

	glfwTerminate();

	return 0;
}

void init()
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
}

void loadShaders()
{
	basic_shader.load("Shaders//Basic//VertexShader.glsl", "Shaders//Basic//FragmentShader.glsl");
	//basic_shader.loadGeometryShader("Shaders//Basic//GeometryShader.glsl");
	simpleDepth_shader.load("Shaders//Basic//ShadowDepthVS.glsl", "Shaders//Basic//ShadowDepthFS.glsl");
	simpleDepth_shader.setUseOnlyDepth(true);
	debugDepthQuad.load("Shaders//Debug//DebugQuadVS.glsl", "Shaders//Debug//DebugQuadFS.glsl");

	lamp_shader.load("Shaders//Lamp//LampVS.glsl", "Shaders//Lamp//LampFS.glsl");
	screen_shader.load("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//ScreenShaderFS.glsl");
	skybox_shader.load("Shaders//Skybox//SkyboxShaderVS.glsl", "Shaders//Skybox//SkyboxShaderFS.glsl");
	final_shader.load("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//FinalShaderFS.glsl");
	shaderBlur.load("Shaders//Blur//BlurShaderVS.glsl", "Shaders//Blur//BlurShaderFS.glsl");
}

void loadTextures()
{
	std::vector<std::string> faces = {
		"Images//Skybox//right.jpg",
		"Images//Skybox//left.jpg",
		"Images//Skybox//top.jpg",
		"Images//Skybox//bottom.jpg",
		"Images//Skybox//back.jpg",
		"Images//Skybox//front.jpg"
	};

	cubemapTexture = loadCubemap(faces, useGammaCorrection);
}

void initBuffers()
{
	// Generate VBO (Vertex Buffer Objects).
	glGenBuffers(1, &VBO);

	// Lighting cube.
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO, the container�s VBO�s data already contains the correct data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set the vertex attributes (only position data for our lamp).
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Floor cube.
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	std::string directory;
	woodTexture = TextureFromFile("floor.jpg", "Images", useGammaCorrection);

	// screen quad VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// generate Framebuffers.
	generateFramebuffer(&framebuffer, &texColorMSBuffer, &rbo, useMultisampling, useHDR);
	generateFramebuffer(&intermediateFBO, &screenTexture, &intermediateRBO, false, useHDR);
	generateBloomFramebuffer(&finalFBO, finalTextures, &finalRBO, useHDR);

	// ping-pong-framebuffer for blurring
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	// Shadow mapping.
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure a uniform buffer object
	// ---------------------------------
	// first. We get the relevant block indices
	unsigned int blockBasicIndex = glGetUniformBlockIndex(basic_shader.getShaderProgram(), "Matrices");
	unsigned int blockLampIndex = glGetUniformBlockIndex(lamp_shader.getShaderProgram(), "Matrices");
	unsigned int blockSkyIndex = glGetUniformBlockIndex(skybox_shader.getShaderProgram(), "Matrices");

	// then we link each shader's uniform block to this uniform binding point
	glUniformBlockBinding(basic_shader.getShaderProgram(), blockBasicIndex, 0);
	glUniformBlockBinding(basic_shader.getShaderProgram(), blockLampIndex, 0);
	glUniformBlockBinding(basic_shader.getShaderProgram(), blockSkyIndex, 0);

	// Now actually create the buffer
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// generate a large list of semi-random model transformation matrices
	// ------------------------------------------------------------------
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed.
	float radius = 150.0;
	float offset = 25.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model;
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}
}

void generateFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useMultisampling, bool useHDR)
{
	glGenFramebuffers(1, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	// generate texture.
	glGenTextures(1, texBuffer);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	if (!useMultisampling)
	{
		glBindTexture(GL_TEXTURE_2D, *texBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		// attach it to currently bound framebuffer object.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texBuffer, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *texBuffer);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, dataFormat, screenWidth, screenHeight, GL_TRUE);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		// attach it to currently bound framebuffer object.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *texBuffer, 0);
	}

	// generate Renderbuffer.
	glGenRenderbuffers(1, RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, *RBO);

	if (!useMultisampling)
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	else
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *RBO);

	// check for complition.
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void generateBloomFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useHDR)
{
	glGenFramebuffers(1, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	glGenTextures(2, texBuffer);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texBuffer[i], 0);
	}

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, *RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *RBO);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void update()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void render()
{
	// Shadow mapping.
	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	glm::vec3 lightPos(30.0f, 20.0f, 10.0f);
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 10.0f, far_plane = 100.0f;

	if (useShadowMapping)
	{
		// note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)shadowWidth / (GLfloat)shadowHeight, near_plane, far_plane);
		lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleDepth_shader.use();
		simpleDepth_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		simpleDepth_shader.setBool("useInstances", false);

		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		drawScene(simpleDepth_shader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, screenWidth, screenHeight);
	}

	// first render scene in specific framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we�re not using the stencil buffer now.

	// Draw Lamp.
	lamp_shader.use();

	glm::mat4 modelMat;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 5000.0f);

	// Store uniform buffer data.
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindVertexArray(lightVAO);

	glm::vec3 lightColors[4];
	lightColors[0] = glm::vec3(2.0f, 2.0f, 2.0f);
	lightColors[1] = glm::vec3(1.5f, 0.0f, 0.0f);
	lightColors[2] = glm::vec3(0.0f, 0.0f, 1.5f);
	lightColors[3] = glm::vec3(0.0f, 1.5f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		modelMat = glm::mat4();
		modelMat = glm::translate(modelMat, pointLightPositions[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.2f));

		lamp_shader.setMat4("model", modelMat);
		lamp_shader.setVec3("lightColor", lightColors[i]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Use our shader program when we want to render an object.
	basic_shader.use();

	// Wireframe mode.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - lightPos);

	basic_shader.setBool("useInstances", false);
	basic_shader.setBool("useShadowMapping", useShadowMapping);
	basic_shader.setFloat("heightScale", 0.1f);
	basic_shader.setBool("material.use_texture_diffuse", true);
	basic_shader.setBool("material.use_texture_specular", false);
	basic_shader.setBool("material.use_texture_normal", false);
	basic_shader.setBool("material.use_texture_height", false);

	basic_shader.setVec4("material.diffuse_color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	basic_shader.setVec4("material.specular_color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	basic_shader.setFloat("material.shininess", 32);

	basic_shader.setVec3("dirLight[0].direction", lightDir);
	basic_shader.setVec3("dirLight[0].ambient", 0.05f, 0.05f, 0.05f);
	basic_shader.setVec3("dirLight[0].diffuse", 1.0f, 1.0f, 1.0f);
	basic_shader.setVec3("dirLight[0].specular", 0.5f, 0.5f, 0.5f);

	for (int i = 0; i < 4; ++i)
	{
		std::string var = "pointLights[" + std::to_string(i) + "].";
		std::string param = var + "position";

		basic_shader.setVec3(param.c_str(), pointLightPositions[i]);
		param = var + "ambient";
		basic_shader.setVec3(param.c_str(), 0.05f, 0.05f, 0.05f);
		param = var + "diffuse";
		basic_shader.setVec3(param.c_str(), lightColors[i]);
		param = var + "specular";
		basic_shader.setVec3(param.c_str(), 1.0f, 1.0f, 1.0f);
		param = var + "constant";
		basic_shader.setFloat(param.c_str(), 1.0f);
		param = var + "linear";
		basic_shader.setFloat(param.c_str(), 0.09f);
		param = var + "quadratic";
		basic_shader.setFloat(param.c_str(), 0.032f);
	}

	basic_shader.setInt("dirCount", 1);
	basic_shader.setInt("pointCount", 4);
	basic_shader.setInt("spotCount", 0);

	basic_shader.setVec3("viewPos", camera.getPosition());
	basic_shader.setVec3("lightPos", lightPos);
	basic_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	basic_shader.setFloat("near_plane", near_plane);
	basic_shader.setFloat("far_plane", far_plane);

	glActiveTexture(GL_TEXTURE0);
	basic_shader.setInt("material.texture_diffuse1", 0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);

	if (useShadowMapping)
	{
		glActiveTexture(GL_TEXTURE4);
		basic_shader.setInt("shadowMap", 4);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}

	drawScene(basic_shader);

	// draw skybox as last.
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content.

	skybox_shader.use();

	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix.

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.

	// second render framebuffer as texture for post-processing.
	// blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO); // back to default
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Here we have to render to final fbo with two outputs.
	final_shader.use();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);

	final_shader.setInt("screenTexture", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Blur here.
	// 2. blur bright fragments with two-pass Gaussian Blur 
	// --------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool horizontal = true, first_iteration = true;
	if (useBloom)
	{
		unsigned int amount = 10;
		shaderBlur.use();

		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur.setInt("horizontal", horizontal);
			shaderBlur.setInt("image", 0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? finalTextures[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	screen_shader.use();
	screen_shader.setBool("gammaCorrection", useGammaCorrection);
	screen_shader.setBool("useHDR", useHDR);
	screen_shader.setInt("useBloom", useBloom);
	screen_shader.setFloat("exposure", 1.0f);

	screen_shader.setInt("screenTexture", 0);
	screen_shader.setInt("bloomTexture", 1);

	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, finalTextures[0]);

	if (useBloom)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Uncomment it for debugging purposes.
	// It will draw current depth map.
	/*
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);
	debugDepthQuad.setFloat("near_plane", near_plane);
	debugDepthQuad.setFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);*/

	glEnable(GL_DEPTH_TEST);
}

void drawScene(Shader shader)
{
	glm::mat4 modelMat;

	// Draw scene.
	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(10.0f, -5.0f, -10.0f));
	modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(100.0f, 100.0f, 0.1f));

	glBindVertexArray(floorVAO);

	shader.setMat4("model", modelMat);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(10.0f, -4.5f, 10.0f));
	modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));

	shader.setMat4("model", modelMat);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// trans - rotate - scale.
	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(5.0f, -5.0f, -10.0f));
	modelMat = glm::rotate(modelMat, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.05f, 0.05f, 0.05f));

	shader.setMat4("model", modelMat);

	model.Draw(shader);

	// draw planet
	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(-20.0f, -3.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(4.0f, 4.0f, 4.0f));
	shader.setMat4("model", modelMat);
	planet.Draw(shader);

	rock.Draw(shader, amount);
}

void clearBuffers()
{
	for(int i = 0; i < 2; ++i)
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;

	resizeFramebuffer(framebuffer, texColorMSBuffer, rbo, useMultisampling, useHDR);
	resizeFramebuffer(intermediateFBO, screenTexture, intermediateRBO, false, useHDR);
	resizeBloomFramebuffer(finalFBO, finalTextures, finalRBO, useHDR);
	for(int i = 0; i < 2; ++i)
		resizeFramebuffer(pingpongFBO[i], pingpongColorbuffers[i], -1, false, useHDR);
}

void resizeFramebuffer(unsigned int FBO, unsigned int texBuffer, unsigned int RBO, bool useMultisampling, bool useHDR)
{
	// Resize framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	if (!useMultisampling)
	{
		glBindTexture(GL_TEXTURE_2D, texBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (RBO != -1)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		glDeleteTextures(1, &texColorMSBuffer);
		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &framebuffer);

		generateFramebuffer(&framebuffer, &texColorMSBuffer, &rbo, useMultisampling, useHDR);
	}
}

void resizeBloomFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO, bool useHDR)
{
	// Resize framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	for (int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseUse) // this bool variable is initially set to true.
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouseUse = false;
	}

	if (lbutton_down)
	{
		float xoffset = xpos - lastMouseX;
		float yoffset = lastMouseY - ypos; // reversed since y-coordinates range from bottom to top.

		camera.ProcessMouseMovement(xoffset, yoffset, true);
	}

	lastMouseX = xpos;
	lastMouseY = ypos;
}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS)
			lbutton_down = true;
		else if (action == GLFW_RELEASE)
			lbutton_down = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}