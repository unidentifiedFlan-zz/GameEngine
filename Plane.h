#pragma once
#include "Mesh.h"
#include "TextureLoader.h"

class Plane : public Mesh
{
	std::vector<float> planeVertices = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};

	glm::vec3 normal = {
		 0.0f, 0.0f, 1.0f
	};

	std::vector<float> planeTextureCoords = {
		 1.0f, 1.0f,
		 1.0f, 0.0f,
		 0.0f, 0.0f,
		 0.0f, 1.0f
	};

	std::vector<unsigned int> indices = {
		0, 1, 3,
		1, 2, 3
	};

public:
	Plane(int numVertices = 2)
	{
		if (numVertices > 2) {
			generateVertices(numVertices);
		}
		getVertices(vertices);
		setVertices(vertices, indices);
	}

	virtual void draw(Shader shader) {
		glDisable(GL_CULL_FACE);
		Mesh::draw(shader);
		glEnable(GL_CULL_FACE);
	}

	void getVertices(std::vector<Vertex> &vertices)
	{
		vertices.clear();

		for (int v = 0, t = 0; v < planeVertices.size() && t < planeTextureCoords.size(); v += 3, t += 2) {
			Vertex vertex;
			glm::vec3 position;

			position.x = planeVertices[v];
			position.y = planeVertices[v + 1];
			position.z = planeVertices[v + 2];
			glm::vec2 glmTextureCoords;
			glmTextureCoords.x = planeTextureCoords[t];
			glmTextureCoords.y = planeTextureCoords[t + 1];

			vertex.Position = position;
			vertex.Normal = normal;
			vertex.TexCoords = glmTextureCoords;
			vertices.push_back(vertex);
		}
	}

	/*
	* Generate uniform plane of vertices to add flexibility
	* numVertices is number of vertices along one direction
	*/
	void generateVertices(int numVertices)
	{
		planeVertices.clear();

		float minVal = -1.0f;
		float maxVal = -minVal;
		float step = (2 * maxVal) / (numVertices-1);

		float x = minVal;
		float y;

		for (int i = 0; i < numVertices; i++) {
			y = minVal;
			for (int j = 0; j < numVertices; j++) {
				planeVertices.insert(planeVertices.end(), { x, y, 0.0f } );
				y += step;
			}
			x += step;
		}

		planeTextureCoords.clear();

		//Here we're stretching the texture across the entire mesh
		minVal = 0.0f, maxVal = 1.0;
		step = maxVal / (numVertices-1);
		x = minVal;

		for (int i = 0; i < numVertices; ++i) {
			y = minVal;
			for (int j = 0; j < numVertices; ++j) {
				planeTextureCoords.push_back(x);
				planeTextureCoords.push_back(y);
				y += step;
			}
			x += step;
		}

		indices.clear();

		unsigned int max = (numVertices - 1)*numVertices;
		for (unsigned int i = 1; i < max; i++) {
			indices.insert(indices.end(), { i, i + numVertices - 1, i - 1});
			indices.insert(indices.end(), { i, i + numVertices, i + numVertices - 1 });

			if ((i + 1) % numVertices == 0) {
				i++;
			}
		}
		
	}

	void setTextureScale(float scaleX, float scaleY)
	{
		for (int i = 0; i < planeTextureCoords.size(); ++i) {
			planeTextureCoords[i] *= i % 2 == 0 ? scaleX : scaleY;
		}

		getVertices(vertices);
		setVertices(vertices, indices);
	}
};