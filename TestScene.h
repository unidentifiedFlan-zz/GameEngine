#pragma once
#include <vector>
#include <Memory>
#include "Scene.h"

class TestScene
{
public:
	static std::shared_ptr<Scene> generate(std::vector<Texture> &textures);

private:
	static std::vector<std::shared_ptr<Light>> createLights(const std::shared_ptr<Camera>& camera);
	static std::vector<std::shared_ptr<RenderObject>> createObjects(std::vector<Texture> &textures);
};

