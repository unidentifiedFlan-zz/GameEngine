#pragma once

#include "RenderObject.h"

class Light : public RenderObject
{
public:
	enum class Type
	{
		Directional,
		Point,
		Spot
	};

	Light(Type type);
	
	void addMesh(const std::shared_ptr<Mesh>& mesh) { _mesh = mesh; }

	Type      type;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
};