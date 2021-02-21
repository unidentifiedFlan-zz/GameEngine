#pragma once
#include "Mesh.h"
#include "TextureLoader.h"


class Cube : public Mesh
{
	std::vector<float> cubeVertices = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f
	};


	std::vector<float> normal = {
		 0.0f, 0.0f, -1.0f,
		 0.0f, 0.0f, -1.0f,
		 0.0f, 0.0f, -1.0f,
		 0.0f, 0.0f, -1.0f,
		 0.0f, 0.0f, -1.0f,
		 0.0f, 0.0f, -1.0f,

		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,

		 -1.0f, 0.0f, 0.0f,
		 -1.0f, 0.0f, 0.0f,
		 -1.0f, 0.0f, 0.0f,
		 -1.0f, 0.0f, 0.0f,
		 -1.0f, 0.0f, 0.0f,
		 -1.0f, 0.0f, 0.0f,

		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,

		 0.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 0.0f,

		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f
	};

	std::vector<float> cubeTextureCoords = {
		 1.0f, 0.0f,
		 0.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f,
		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 0.0f, 1.0f,
		 1.0f, 1.0f
	};

	std::vector<unsigned int> indices = {
		3, 1, 0,
		0, 2, 3,
		4, 5, 7,
		7, 6, 4,
		6, 2, 0,
		0, 4, 6,
		1, 3, 7,
		7, 5, 1,
		0, 1, 5,
		5, 4, 0,
		7, 3, 2,
		2, 6, 7
	};

public:
	Cube()
	{
		getVertices(vertices);
		setVertices(vertices, indices);
	}

	void getVertices(std::vector<Vertex> &vertices)
	{
		vertices.clear();

		for (int v = 0, n = 0, t = 0; v < cubeVertices.size() && t < cubeTextureCoords.size(); v += 3, n++, t += 2) {
			Vertex vertex;
			glm::vec3 position;

			position.x = cubeVertices[v];
			position.y = cubeVertices[v + 1];
			position.z = cubeVertices[v + 2];
			glm::vec2 glmTextureCoords;
			glmTextureCoords.x = cubeTextureCoords[t];
			glmTextureCoords.y = cubeTextureCoords[t + 1];

			vertex.Position = position;
			vertex.Normal = glm::vec3(normal[n], normal[n+1], normal[n+2]);
			vertex.TexCoords = glmTextureCoords;
			vertices.push_back(vertex);
		}
	}

	void setTextureScale(float scaleX, float scaleY)
	{
		for (int i = 0; i < cubeTextureCoords.size(); ++i) {
			cubeTextureCoords[i] *= i % 2 == 0 ? scaleX : scaleY;
		}

		getVertices(vertices);
		setVertices(vertices, indices);
	}
};