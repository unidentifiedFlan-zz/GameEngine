#pragma once
#include "Mesh.h"
#include "PhysicalObject.h"
#include <vector>
#include "RenderObject.h"

struct Line
{
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 direction;
	float length;
};

/*
	A tool to generate PhysicalObject blocks along the surface of a mesh
	such that each point on the surface lies within a block. For a given
	block side length, the longest length of each surface triangle on the
	mesh is traversed in increments of the block size. Lines are calculated
	from the incremented points on the longest side to the closest of the
	other two sides. This line is similarly incremented a PhysicalObject
	created at that position
*/

class PhysicalObjectGenerator
{
	std::vector<std::vector<std::vector<PhysicalObject*>>> &_grid;
	std::vector<Line> _currentLines;
	float _blockSize;
	glm::vec3 _offset;

public:
	PhysicalObjectGenerator(std::vector<std::vector<std::vector<PhysicalObject*>>> &grid, float blockSize, glm::vec3 offset);
	void GenerateFromMeshSurface(const Mesh &mesh);

private:
	void GeneratePhysicalSurface(const Mesh &mesh);
	void GeneratePhysicalTriangle();
	void GeneratePhysicalLine(const glm::vec3 &start, const glm::vec3 &direction, const glm::vec3 &end);
	std::vector<Line> GetOrderedTriangleLines(const Vertex &v1, const Vertex &v2, const Vertex &v3) const;
	Line GetLine(const Vertex &v1, const Vertex &v2) const;
};

