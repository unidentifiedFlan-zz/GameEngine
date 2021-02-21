#pragma once
#include <vector>
#include <string>
#include "RenderObject.h"
#include "TextureLoader.h"
#include "TinyOBJLoader/tiny_obj_loader.h"

class OBJLoader
{
public:
	OBJLoader(TextureLoader &textureLoader);
	std::shared_ptr<RenderObject> load(const std::string &path);

private:
	std::shared_ptr<RenderObject> processNode(tinyobj::attrib_t attrib,
		std::vector<tinyobj::shape_t> shapes,
		std::vector<tinyobj::material_t> materials);
	std::shared_ptr<RenderObject> processMesh(tinyobj::attrib_t attrib,
		tinyobj::shape_t mesh,
		std::vector<tinyobj::material_t> materials);

	std::string directory;
	TextureLoader &_textureLoader;
};

