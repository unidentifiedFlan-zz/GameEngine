#include "Scene.h"
#include "EventHandler.h"
#include <iostream>

Scene::Scene()
	:
	_pointShadowsUninitialised(false),
	_directionalShadowsUninitialised(false),
	_nearPlane(1.0f),
	_farPlane(30.0f)
{
	glEnable(GL_DEPTH_TEST);
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &_pointShadowMaps);
	glGenTextures(1, &_directionalShadowMaps);
}

void Scene::initialisePointShadowMap()
{
	unsigned int layers = 6 *_pointLights.size();
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _pointShadowMaps);
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

	_pointShadowsUninitialised = false;
}

void Scene::initialiseDirectionalShadowMap()
{
	unsigned int layers = _directionalLights.size();
	glBindTexture(GL_TEXTURE_2D_ARRAY, _directionalShadowMaps);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	_directionalShadowsUninitialised = false;
}

void Scene::addObject(const std::shared_ptr<RenderObject>& object)
{
	_objects.push_back(object);
}

void Scene::addObjects(const std::vector<std::shared_ptr<RenderObject>>& objects)
{
	_objects.insert(_objects.end(), objects.begin(), objects.end());
}

void Scene::addLight(const std::shared_ptr<Light>& light)
{
	if (light->type == Light::Type::Point) {
		_pointLights.push_back(light);
		_pointShadowsUninitialised = true;
	}

	if (light->type == Light::Type::Directional) {
		_directionalLights.push_back(light);
		_lightSpaceMatrix.push_back(glm::mat4(0.0f));
		_directionalShadowsUninitialised = true;
	}
}

void Scene::addLights(const std::vector<std::shared_ptr<Light>>& lights)
{
	for (const auto& light : lights) {
		if (light->type == Light::Type::Point) {
			_pointLights.push_back(light);
			_pointShadowsUninitialised = true;
		}

		if (light->type == Light::Type::Directional) {
			_directionalLights.push_back(light);
			_lightSpaceMatrix.push_back(glm::mat4(0.0f));
			_directionalShadowsUninitialised = true;
		}
	}
}

void Scene::setLightSource(const std::shared_ptr<Light>& light, Shader & shader, unsigned int &lightNum)
{
	if (light->type == Light::Type::Directional) {
		setDirectionalLight(std::dynamic_pointer_cast<DirectionalLight>(light), shader, lightNum);
		lightNum++;
	}
	else if (light->type == Light::Type::Point) {
		setPointLight(std::dynamic_pointer_cast<PointLight>(light), shader, lightNum);
		lightNum++;
	}
	else if (light->type == Light::Type::Spot) {
		setSpotLight(std::dynamic_pointer_cast<SpotLight>(light), shader);
		shader.setBool("spotLight.deactivated", false);
	}
}

void Scene::setSpotLight(const std::shared_ptr<SpotLight>& light, Shader & shader)
{
	shader.setVec3("spotLight.position", light->transform.translation);
	shader.setVec3("spotLight.direction", light->direction);
	shader.setVec3("spotLight.ambient", light->ambient);
	shader.setVec3("spotLight.diffuse", light->diffuse);
	shader.setVec3("spotLight.specular", light->specular);
	shader.setFloat("spotLight.constant", light->constant);
	shader.setFloat("spotLight.linear", light->linear);
	shader.setFloat("spotLight.quadratic", light->quadratic);
	shader.setFloat("spotLight.cutOff", light->cutOff);
	shader.setFloat("spotLight.outerCutOff", light->outerCutOff);
}

void Scene::setPointLight(const std::shared_ptr<PointLight>& light, Shader & shader, unsigned int pointLightNum)
{
	std::string lightElement = "pointLights[" + std::to_string(pointLightNum) + "]";

	shader.setVec3(lightElement + ".position", light->transform.translation);
	shader.setVec3(lightElement + ".ambient", light->ambient);
	shader.setVec3(lightElement + ".diffuse", light->diffuse);
	shader.setVec3(lightElement + ".specular", light->specular);
	shader.setFloat(lightElement + ".constant", light->constant);
	shader.setFloat(lightElement + ".linear", light->linear);
	shader.setFloat(lightElement + ".quadratic", light->quadratic);
}

void Scene::setDirectionalLight(const std::shared_ptr<DirectionalLight>& light, Shader & shader, unsigned int directionalLightNum)
{
	std::string lightElement = "directionalLight[" + std::to_string(directionalLightNum) + "]";

	shader.setVec3(lightElement + ".direction", light->direction);
	shader.setVec3(lightElement + ".ambient", light->ambient);
	shader.setVec3(lightElement + ".diffuse", light->diffuse);
	shader.setVec3(lightElement + ".specular", light->specular);
}

void Scene::draw(float aspectRatio)
{
	if (_pointShadowsUninitialised) {
		initialisePointShadowMap();
	}
	if (_directionalShadowsUninitialised) {
		initialiseDirectionalShadowMap();
	}

	if (_pointLights.size() > 0) {
		generatePointShadowMap(aspectRatio);
	}
	if (_directionalLights.size() > 0) {
		generateDirectionalShadowMap(aspectRatio);
	}

	glm::mat4 view = _camera->getViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(_camera->zoom()), aspectRatio, _nearPlane, _farPlane);

	drawObjects(view, projection, aspectRatio);

	for (auto& light : _pointLights) {
		light->material.shader.activate();
		light->material.shader.setMat4("view", view);
		light->material.shader.setMat4("projection", projection);
		light->draw(light->material.shader);
	}
}

void Scene::drawGeometryPass(Shader &shader, float aspectRatio)
{
	glm::mat4 view = _camera->getViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(_camera->zoom()), aspectRatio, _nearPlane, _farPlane);

	for (auto& obj : _objects) {
		shader.activate();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		obj->draw(shader);
	}
}

void Scene::drawShadowMap(float aspectRatio)
{
	if (_pointShadowsUninitialised) {
		initialisePointShadowMap();
	}
	if (_directionalShadowsUninitialised) {
		initialiseDirectionalShadowMap();
	}

	if (_pointLights.size() > 0) {
		generatePointShadowMap(aspectRatio);
	}
	if (_directionalLights.size() > 0) {
		generateDirectionalShadowMap(aspectRatio);
	}
}

void Scene::registerLights(Shader &shader, float aspectRatio)
{
	glm::mat4 view = _camera->getViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(_camera->zoom()), aspectRatio, _nearPlane, _farPlane);

	shader.activate();
	shader.setVec3("viewPos", _camera->position);

	unsigned int pointLightNum = 0;
	unsigned int directionalLightNum = 0;
	shader.setBool("spotLight.deactivated", true);

	for (int i = 0; i < _pointLights.size(); ++i) {
		setLightSource(_pointLights[i], shader, pointLightNum);
	}

	for (int i = 0; i < _directionalLights.size(); ++i) {
		setLightSource(_directionalLights[i], shader, directionalLightNum);
		shader.setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", _lightSpaceMatrix[i]);
	}

	addPointShadowMap(shader, 4);
	addDirectionalShadowMap(shader, 5);

	shader.setInt("NUM_POINT_LIGHTS", _pointLights.size());
	shader.setInt("NUM_DIRECTIONAL_LIGHTS", _directionalLights.size());
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
}

void Scene::drawLights(Shader &shader, float aspectRatio)
{
	glm::mat4 view = _camera->getViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(_camera->zoom()), aspectRatio, _nearPlane, _farPlane);

	for (auto& light : _pointLights) {
		shader.activate();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("lightColor", light->diffuse);
		light->draw(shader);
	}
}

void Scene::generateDirectionalShadowMap(float aspectRatio)
{
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, _nearPlane, _farPlane);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	Shader shadowShader("DirectionalShadowDepth.vs", "DirectionalShadowDepth.fs");
	shadowShader.activate();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	for (int i = 0; i < _directionalLights.size(); ++i) {

		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _directionalShadowMaps, 0, i);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Could not create FrameBuffer: " << status << std::endl;
		}

		glClear(GL_DEPTH_BUFFER_BIT);

		auto light = std::dynamic_pointer_cast<DirectionalLight>(_directionalLights[i]);
		glm::mat4 lightView = glm::lookAt(light->transform.translation,
			light->transform.translation + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));
		_lightSpaceMatrix[i] = lightProjection * lightView;

		shadowShader.setMat4("lightSpaceMatrix", _lightSpaceMatrix[i]);

		drawObjects(glm::mat4(0.0), glm::mat4(0.0), 0, &shadowShader);
	}

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ResetViewPortEvent e;
	EventHandler<ResetViewPortEvent>::instance()->raise(e);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::generatePointShadowMap(float aspectRatio)
{
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, _nearPlane, _farPlane);

	Shader shadowShader("ShadowDepth.vs", "ShadowDepth.fs", "ShadowDepth.gs");

	shadowShader.activate();

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _pointShadowMaps, 0);
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Could not create FrameBuffer: " << status << std::endl;
	}
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glClear(GL_DEPTH_BUFFER_BIT);

	shadowShader.setFloat("far_plane", _farPlane);
	shadowShader.setInt("NUM_POINT_LIGHTS", _pointLights.size());

	for (int i = 0; i < _pointLights.size(); ++i) {
		shadowShader.setVec3("lightPositions[" + std::to_string(i) + "]", _pointLights[i]->transform.translation);

		std::vector<glm::mat4> shadowTransforms;
		glm::vec3 lightPos = _pointLights[i]->transform.translation;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		for (unsigned int j = 0; j < 6; ++j) {
			shadowShader.setMat4("shadowMatrices[" + std::to_string(i*6 +j) + "]", shadowTransforms[j]);
		}
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	drawObjects(glm::mat4(0.0), glm::mat4(0.0), 0, &shadowShader);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ResetViewPortEvent e;
	EventHandler<ResetViewPortEvent>::instance()->raise(e);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::drawObjects(glm::mat4 view, glm::mat4 projection, float aspectRatio, Shader *override)
{
	for (auto& obj : _objects) {
		Shader shader = override ? *override : obj->material.shader;
		shader.activate();
		shader.setVec3("viewPos", _camera->position);

		unsigned int pointLightNum = 0;
		unsigned int directionalLightNum = 0;
		shader.setBool("spotLight.deactivated", true);

		unsigned int numTex = obj->numberOfTextures();

		for (int i = 0; i < _pointLights.size(); ++i) {
			setLightSource(_pointLights[i], shader, pointLightNum);
		}

		for (int i = 0; i < _directionalLights.size(); ++i) {
			setLightSource(_directionalLights[i], shader, directionalLightNum);
			shader.setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", _lightSpaceMatrix[i]);
		}

		addPointShadowMap(shader, numTex+1);
		addDirectionalShadowMap(shader, numTex+2);

		shader.setInt("NUM_POINT_LIGHTS", _pointLights.size());
		shader.setInt("NUM_DIRECTIONAL_LIGHTS", _directionalLights.size());
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		obj->draw(shader);
	}
}

void Scene::addPointShadowMap(Shader &shader, unsigned int textureNum)
{
	shader.setFloat("far_plane", _farPlane);
	shader.setInt("shadowCubeMap", textureNum);
	glActiveTexture(GL_TEXTURE0 + 0x0001 * (textureNum));
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _pointShadowMaps);
}

void Scene::addDirectionalShadowMap(Shader &shader, unsigned int textureNum)
{
	shader.setInt("shadowDirectionalMap", textureNum);
	glActiveTexture(GL_TEXTURE0 + 0x0001 * (textureNum));
	glBindTexture(GL_TEXTURE_2D_ARRAY, _directionalShadowMaps);
}
