#include "Shader.h"
#include "Camera.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void init();
void loadShaders();
void initBuffers();
void clearBuffers();
void update();
void render();

int screenWidth, screenHeight;

float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};
unsigned int indices[] = { // note that we start from 0!
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};

unsigned int VBO;
unsigned int EBO;

unsigned int lightVAO;

unsigned int diffuseMap;
unsigned int specularMap;

Shader basic_shader;
Shader lamp_shader;
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

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Uncomment this if uses Mac OS X.
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	screenWidth = 800;
	screenHeight = 600;

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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
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

	//uncomment it if not using models loading.
	//stbi_set_flip_vertically_on_load(true);

	loadShaders();
	initBuffers();

	model.loadModel("Models//house_01.obj");
}

void loadShaders()
{
	basic_shader.load("Shaders//Basic//VertexShader.glsl", "Shaders//Basic//FragmentShader.glsl");
	lamp_shader.load("Shaders//Lamp//LampVS.glsl", "Shaders//Lamp//LampFS.glsl");
}

void initBuffers()
{
	// Generate VBO (Vertex Buffer Objects).
	glGenBuffers(1, &VBO);
	glEnable(GL_DEPTH_TEST);

	// Lighting cube.
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO, the container’s VBO’s data already contains the correct data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set the vertex attributes (only position data for our lamp).
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void update()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void render()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	basic_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	basic_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	basic_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	basic_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

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

	basic_shader.setVec3("spotLight.position", camera.getPosition());
	basic_shader.setVec3("spotLight.direction", camera.getFront());
	basic_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	basic_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	basic_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	basic_shader.setFloat("spotLight.constant", 1.0f);
	basic_shader.setFloat("spotLight.linear", 0.09);
	basic_shader.setFloat("spotLight.quadratic", 0.032);
	basic_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	basic_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	/*
	basic_shader.setInt("material.diffuse", 0);
	basic_shader.setInt("material.specular", 1);
	basic_shader.setFloat("material.shininess", 32.0f);*/

	//basic_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	basic_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	basic_shader.setVec3("viewPos", camera.getPosition());

	modelMat = glm::mat4();
	//modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.1f, 0.1f, 0.1f));

	modelLoc = glGetUniformLocation(basic_shader.getShaderProgram(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

	viewLoc = glGetUniformLocation(basic_shader.getShaderProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	projLoc = glGetUniformLocation(basic_shader.getShaderProgram(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	model.Draw(basic_shader);
}

void clearBuffers()
{
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
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

	float xoffset = xpos - lastMouseX;
	float yoffset = lastMouseY - ypos; // reversed since y-coordinates range from bottom to top.

	lastMouseX = xpos;
	lastMouseY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}