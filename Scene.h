#pragma once
#include "RenderObject.h"
#include "Camera.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"

class Scene
{
public:
	Scene();
	unsigned int directionalShadowMaps() { return _directionalShadowMaps; }

	void addObject(const std::shared_ptr<RenderObject>& object);
	void addObjects(const std::vector<std::shared_ptr<RenderObject>>& objects);
	void addLight(const std::shared_ptr<Light>& light);
	void addLights(const std::vector<std::shared_ptr<Light>>& lights);
	void setCamera(const std::shared_ptr<Camera>& camera) { _camera = camera; }
	const std::shared_ptr<Camera>& camera() { return _camera; }
	void draw(float aspectRatio);
	void drawGeometryPass(Shader &shader, float aspectRatio);
	void drawShadowMap(float aspectRatio);
	void registerLights(Shader &shader, float aspectRatio);
	void drawLights(Shader &shader, float aspectRatio);

private:
	void initialisePointShadowMap();
	void initialiseDirectionalShadowMap();
	void setLightSource(const std::shared_ptr<Light>& light, Shader & shader, unsigned int &pointLightNum);
	void setSpotLight(const std::shared_ptr<SpotLight>& light, Shader & shader);
	void setPointLight(const std::shared_ptr<PointLight>& light, Shader & shader, unsigned int pointLightNum);
	void setDirectionalLight(const std::shared_ptr<DirectionalLight>& light, Shader & shader, unsigned int directionalLightNum);
	void generateDirectionalShadowMap(float aspectRatio);
	void generatePointShadowMap(float aspectRatio);
	void drawObjects(glm::mat4 view, glm::mat4 projection, float aspectRatio, Shader *shader = nullptr);
	void addPointShadowMap(Shader &shader, unsigned int textureNum);
	void addDirectionalShadowMap(Shader &shader, unsigned int textureNum);

	std::vector<std::shared_ptr<RenderObject>> _objects;
	std::vector<std::shared_ptr<Light>>        _pointLights;
	std::vector<std::shared_ptr<Light>>        _directionalLights;
	std::vector<glm::mat4>     _lightSpaceMatrix;
	std::shared_ptr<Camera>    _camera;
	const unsigned int         SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int               depthMapFBO;
	unsigned int               _directionalShadowMaps;
	unsigned int               _pointShadowMaps;
	const float                _nearPlane;
	const float                _farPlane;
	bool                       _pointShadowsUninitialised;
	bool                       _directionalShadowsUninitialised;
};

