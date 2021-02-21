#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
};

class Mesh
{
public:
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;

	Mesh() {}
	Mesh( std::vector<Vertex> &vertices, std::vector<unsigned int> &indices );
	void setVertices( std::vector<Vertex> &vertices, std::vector<unsigned int> &indices );
	virtual void draw(Shader shader);
	
private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
	void calculateTangentVectors();
};

