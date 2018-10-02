#include "MainApp.h"

void MainApp::processInput(GLFWwindow *window)
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

void MainApp::processMouseMove(GLFWwindow* window, double xpos, double ypos)
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

void MainApp::processMouseClick(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS)
			lbutton_down = true;
		else if (action == GLFW_RELEASE)
			lbutton_down = false;
	}
}

void MainApp::processMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}