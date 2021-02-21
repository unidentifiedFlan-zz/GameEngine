#pragma once
#include "Texture.h"

class Material
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Texture diffuseMap;
	Texture specularMap;
	Texture normalMap;

	Shader shader;

	Material()
		:
		ambient(glm::vec3(1.0f, 1.0f, 1.0f)),
		diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
		specular(glm::vec3(0.1f, 0.1f, 0.1f)),
		shininess(8.0f),
		shader(Shader("shader.vs", "shader.fs"))
	{
	}
};