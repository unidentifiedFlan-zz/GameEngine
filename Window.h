#pragma once
#include "Camera.h">
#include <GLFW/glfw3.h>
#include <functional>
#include <memory>

struct MouseUpdateEvent
{
	double xPos;
	double yPos;
};

struct ScrollUpdateEvent
{
	double xOffset;
	double yOffset;
};

class Window
{
	unsigned int SCREEN_WIDTH, SCREEN_HEIGHT;
	float lastX;
	float lastY;
	bool firstMouse;
	float deltaTime;
	float lastFrame;
	std::shared_ptr<Camera> _camera;
	GLFWwindow *_window;

public:
	Window(unsigned int width = 800, unsigned int height = 600);
	float aspectRatio() { return (float)SCREEN_WIDTH / SCREEN_HEIGHT; }
	unsigned int width() { return SCREEN_WIDTH; }
	unsigned int height() { return SCREEN_HEIGHT; }
	void setCamera(const std::shared_ptr<Camera>& camera);
	void updateInput();
	void update();
	bool closed() const;
	~Window();

private:
	void addListeners();
	void removeListeners();
	void updateMouse(const MouseUpdateEvent &e);
	void updateScroll(const ScrollUpdateEvent &e);
	void resetViewPort(const ResetViewPortEvent &e);
	void initGlad();

	static void mouse_callback(GLFWwindow *window, double xoffset, double yoffset);
	static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
	static void framebufferSize_callback(GLFWwindow *window, int width, int height);
};

