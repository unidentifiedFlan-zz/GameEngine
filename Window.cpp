#include "Window.h"
#include <glad/glad.h>
#include <iostream>
#include "EventHandler.h"

Window::Window(unsigned int width, unsigned int height)
	:
	SCREEN_WIDTH(width),
	SCREEN_HEIGHT(height),
	lastX(SCREEN_WIDTH / 2.0f),
	lastY(SCREEN_HEIGHT / 2.0f),
	firstMouse(true),
	deltaTime(0.0f),
	lastFrame(0.0f)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "StrategyGame", NULL, NULL);
	if (_window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
	}
	glfwMakeContextCurrent(_window);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(_window, &mouse_callback);
	glfwSetScrollCallback(_window, &scroll_callback);

	initGlad();

	glfwSetFramebufferSizeCallback(_window, &framebufferSize_callback);

	addListeners();
}

void Window::addListeners()
{
	EventHandler<MouseUpdateEvent>::instance()->addListener(&Window::updateMouse, this);
	EventHandler<ScrollUpdateEvent>::instance()->addListener(&Window::updateScroll, this);
	EventHandler<ResetViewPortEvent>::instance()->addListener(&Window::resetViewPort, this);
}

void Window::removeListeners()
{
	EventHandler<MouseUpdateEvent>::instance()->removeListener(&Window::updateMouse, this);
	EventHandler<ScrollUpdateEvent>::instance()->removeListener(&Window::updateScroll, this);
	EventHandler<ResetViewPortEvent>::instance()->removeListener(&Window::resetViewPort, this);

}

void Window::setCamera(const std::shared_ptr<Camera>& camera)
{
	_camera = camera;
}

void Window::updateMouse(const MouseUpdateEvent &e)
{
	if (firstMouse)
	{
		lastX = e.xPos;
		lastY = e.yPos;
		firstMouse = false;
	}

	float xoffset = e.xPos - lastX;
	float yoffset = e.yPos - lastY;
	lastX = e.xPos;
	lastY = e.yPos;

	_camera->processMouseMovement(xoffset, yoffset);
}

void Window::updateScroll(const ScrollUpdateEvent &e)
{
	_camera->processMouseScroll(e.yOffset);
}

void Window::resetViewPort(const ResetViewPortEvent &e)
{
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Window::updateInput()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, true);
	}
	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		_camera->processKeyboard(Camera::FORWARD, deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		_camera->processKeyboard(Camera::BACKWARD, deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		_camera->processKeyboard(Camera::LEFT, deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		_camera->processKeyboard(Camera::RIGHT, deltaTime);
}

void Window::update()
{
	glfwSwapBuffers(_window);
	glfwPollEvents();
}

bool Window::closed() const
{
	return glfwWindowShouldClose(_window);
}

void Window::initGlad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD" << std::endl;
	}
}

void Window::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	MouseUpdateEvent e = { xpos, ypos };
	EventHandler<MouseUpdateEvent>::instance()->raise(e);
}

void Window::scroll_callback(GLFWwindow * window, double xpos, double ypos)
{
	ScrollUpdateEvent e = { xpos, ypos };
	EventHandler<ScrollUpdateEvent>::instance()->raise(e);
}

void Window::framebufferSize_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Window::~Window()
{
	removeListeners();
	glfwTerminate();
}
