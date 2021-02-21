#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "Scene.h"
#include "Window.h"
#include "Renderer.h"
#include "TestScene.h"
#include "EventHandler.h"

#include "Plane.h"

void setDefaultGLTests()
{
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
	glStencilMask(0xFF); // enable writing to the stencil buffer
	glEnable(GL_DEPTH_TEST);
}

void setInnerStencil()
{
	setDefaultGLTests();
}

void setOuterStencil()
{
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0x00);
}

void setOutlineStencil()
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00); // disable writing to the stencil buffer
	glDisable(GL_DEPTH_TEST);
}

void startGame(Window &window);

int main()
{
	Window window;
	startGame(window);

	return 0;
}

void startGame(Window &window)
{
	std::vector<Texture> textures;

	std::shared_ptr<Scene> scene = TestScene::generate(textures);
	window.setCamera(scene->camera());
/*
	SpotLight * sLight = new SpotLight();
	sLight->ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	sLight->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	sLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	sLight->constant = 1.0f;
	sLight->linear = 0.09;
	sLight->quadratic = 0.032;
	sLight->cutOff = glm::cos(glm::radians(12.5f));
	sLight->outerCutOff = glm::cos(glm::radians(15.0f));

	scene->addLight(sLight);
*/	

	Renderer renderer;
	while (!window.closed()) {

		window.updateInput();
//		sLight->transform.translate(scene->camera()->position);
//		sLight->direction = scene->camera()->front;
		renderer.render(*scene, window.width(), window.height());
		window.update();
		FrameUpdateEvent e;
		e.numFrames = 1;
		EventHandler<FrameUpdateEvent>::instance()->raise(e);
	}
}