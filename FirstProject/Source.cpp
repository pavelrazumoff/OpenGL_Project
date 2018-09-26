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

int screenWidth, screenHeight;

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
unsigned int framebuffer;
unsigned int texColorBuffer;
unsigned int rbo;

unsigned int lightVAO;
unsigned int quadVAO, quadVBO;
unsigned int skyboxVAO, skyboxVBO;

unsigned int diffuseMap;
unsigned int specularMap;
unsigned int cubemapTexture;

Shader basic_shader;
Shader lamp_shader;
Shader screen_shader;
Shader skybox_shader;

Camera camera;
Model model;

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
	//uncomment it if not using models loading.
	//stbi_set_flip_vertically_on_load(true);

	loadShaders();
	loadTextures();
	initBuffers();

	model.loadModel("Models//house_01.obj");
}

void loadShaders()
{
	basic_shader.load("Shaders//Basic//VertexShader.glsl", "Shaders//Basic//FragmentShader.glsl");
	lamp_shader.load("Shaders//Lamp//LampVS.glsl", "Shaders//Lamp//LampFS.glsl");
	screen_shader.load("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//ScreenShaderFS.glsl");
	skybox_shader.load("Shaders//Skybox//SkyboxShaderVS.glsl", "Shaders//Skybox//SkyboxShaderFS.glsl");
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

	cubemapTexture = loadCubemap(faces);
}

void initBuffers()
{
	// Generate VBO (Vertex Buffer Objects).
	glGenBuffers(1, &VBO);

	// Lighting cube.
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO, the container’s VBO’s data already contains the correct data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set the vertex attributes (only position data for our lamp).
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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

	// generate Framebuffer.
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// generate texture.
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// generate Renderbuffer.
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// check for complition.
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
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
	// first render scene in specific framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we’re not using the stencil buffer now.
	glEnable(GL_DEPTH_TEST);

	// Draw Lamp.
	lamp_shader.use();

	glm::mat4 modelMat;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 5000.0f);

	glBindVertexArray(lightVAO);

	int modelLoc = glGetUniformLocation(lamp_shader.getShaderProgram(), "model");

	int viewLoc = glGetUniformLocation(lamp_shader.getShaderProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	int projLoc = glGetUniformLocation(lamp_shader.getShaderProgram(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	for (int i = 0; i < 4; ++i)
	{
		modelMat = glm::mat4();
		modelMat = glm::translate(modelMat, pointLightPositions[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.2f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Draw scene.
	// Use our shader program when we want to render an object.
	basic_shader.use();

	// Wireframe mode.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	basic_shader.setVec3("dirLight[0].direction", -0.2f, -1.0f, -0.3f);
	basic_shader.setVec3("dirLight[0].ambient", 0.05f, 0.05f, 0.05f);
	basic_shader.setVec3("dirLight[0].diffuse", 0.4f, 0.4f, 0.4f);
	basic_shader.setVec3("dirLight[0].specular", 0.5f, 0.5f, 0.5f);

	for (int i = 0; i < 4; ++i)
	{
		std::string var = "pointLights[" + std::to_string(i) + "].";
		std::string param = var + "position";

		basic_shader.setVec3(param.c_str(), pointLightPositions[i]);
		param = var + "ambient";
		basic_shader.setVec3(param.c_str(), 0.05f, 0.05f, 0.05f);
		param = var + "diffuse";
		basic_shader.setVec3(param.c_str(), 0.8f, 0.8f, 0.8f);
		param = var + "specular";
		basic_shader.setVec3(param.c_str(), 1.0f, 1.0f, 1.0f);
		param = var + "constant";
		basic_shader.setFloat(param.c_str(), 1.0f);
		param = var + "linear";
		basic_shader.setFloat(param.c_str(), 0.09f);
		param = var + "quadratic";
		basic_shader.setFloat(param.c_str(), 0.032f);
	}
	/*
	basic_shader.setVec3("spotLight[0].position", camera.getPosition());
	basic_shader.setVec3("spotLight[0].direction", camera.getFront());
	basic_shader.setVec3("spotLight[0].ambient", 0.0f, 0.0f, 0.0f);
	basic_shader.setVec3("spotLight[0].diffuse", 1.0f, 1.0f, 1.0f);
	basic_shader.setVec3("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
	basic_shader.setFloat("spotLight[0].constant", 1.0f);
	basic_shader.setFloat("spotLight[0].linear", 0.09);
	basic_shader.setFloat("spotLight[0].quadratic", 0.032);
	basic_shader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(12.5f)));
	basic_shader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));*/

	basic_shader.setInt("dirCount", 1);
	basic_shader.setInt("pointCount", 4);
	basic_shader.setInt("spotCount", 0);

	basic_shader.setVec3("viewPos", camera.getPosition());

	// trans - rotate - scale.
	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(5.0f, -5.0f, -10.0f));
	modelMat = glm::rotate(modelMat, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.05f, 0.05f, 0.05f));

	modelLoc = glGetUniformLocation(basic_shader.getShaderProgram(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

	viewLoc = glGetUniformLocation(basic_shader.getShaderProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	projLoc = glGetUniformLocation(basic_shader.getShaderProgram(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	model.Draw(basic_shader);

	// draw skybox as last.
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content.

	skybox_shader.use();

	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix.

	skybox_shader.setMat4("view", view);
	skybox_shader.setMat4("projection", projection);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.

	// second render framebuffer as texture for post-processing.
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	screen_shader.use();

	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void clearBuffers()
{
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &skyboxVBO);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;

	// Resize framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

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