#include "PhysicalObjectGenerator.h"
#include <algorithm>
#include "OBJLoader.h"

PhysicalObjectGenerator::PhysicalObjectGenerator(std::vector<std::vector<std::vector<PhysicalObject*>>> &grid,
												 float blockSize,
												 glm::vec3 offset)
	:
	_grid(grid),
	_blockSize(blockSize),
	_offset(offset)
{
}

void PhysicalObjectGenerator::GenerateFromMeshSurface(const Mesh &mesh)
{
	if (mesh.indices.size()%3 != 0)
		return;

	try
	{
		GeneratePhysicalSurface(mesh);
	}
	catch (std::exception &e)
	{
		std::cout << "Failed to generate physical surface: " << e.what() << std::endl;
	}
}

void PhysicalObjectGenerator::GeneratePhysicalSurface(const Mesh &mesh)
{
	for (unsigned int i = 0; i < mesh.indices.size(); i += 3)
	{
		_currentLines = GetOrderedTriangleLines(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
		GeneratePhysicalTriangle();
		return;
	}
}

void PhysicalObjectGenerator::GeneratePhysicalTriangle()
{
	Line &longestLine = _currentLines.back();

	Line &secondConstraint = _currentLines[0].start == longestLine.end ? _currentLines[0] : _currentLines[1];
	Line &firstConstraint = _currentLines[0].start == longestLine.end ? _currentLines[1] : _currentLines[0];

	float firstCosine = glm::dot(longestLine.direction, -firstConstraint.direction);
	float secondCosine = glm::dot(longestLine.direction, secondConstraint.direction);
	/*
		Given the direction of a longer line, we want the line running perpendicular to the
		longest line, meeting the end of the constraint line (the dashed line below, where
		the dotted line is the constraint line):

	direction   x start
		^		xo
		|		x  o
		|		x    o
		|		x - - o end
				x
	*/
	glm::vec3 intersectionPoint = secondConstraint.start - secondConstraint.direction * secondCosine;
	float intersectionDistFromStart = glm::length(intersectionPoint - longestLine.start);
	glm::vec3 perpDirection = glm::normalize(secondConstraint.end - intersectionPoint);

	/*
		Need to find point on x-line intersected by perpendicular to long-dashed line.
		If Xcos(theta) = Y, then end = start + X.direction * ( Y/cos(theta))

		|-------x end
		|     x
	  Y |   x  X
		| x
		x start
	*/

	glm::vec3 increment = _blockSize * longestLine.direction;
	float margin = 0.5f*_blockSize;

	glm::vec3 currentPos = longestLine.start;
	bool cond = false;

	//for(glm::vec3 currentPos = longestLine.start; glm::length(currentPos - longestLine.end) <= margin; currentPos += increment)
	while(!cond)
	{
		glm::vec3 end;

		if (glm::length(currentPos - longestLine.start) < intersectionDistFromStart)
		{
			//Negative because the first constraints direction is toward the start
			//of the longest line
			glm::vec3 firstVectorToEnd = -firstConstraint.direction / firstCosine;
			float distFromStart = glm::length(currentPos - longestLine.start);
			end = longestLine.start +  distFromStart * firstVectorToEnd;
		}
		else
		{
			glm::vec3 secondVectorToEnd = secondConstraint.direction / secondCosine;
			float distFromEnd = glm::length(currentPos - longestLine.end);
			end = longestLine.end + distFromEnd * secondVectorToEnd;
		}

		GeneratePhysicalLine(currentPos, perpDirection, end);
		currentPos += increment;
		cond =  glm::length(currentPos - longestLine.end) <= margin;
	}
}

void PhysicalObjectGenerator::GeneratePhysicalLine(const glm::vec3 &start, const glm::vec3 &direction, const glm::vec3 &end)
{
	float margin = 0.5f*_blockSize;
	glm::vec3 increment = _blockSize * direction;

	glm::vec3 currentPos = start;
	do {
		glm::vec3 index = glm::floor((currentPos - _offset)/_blockSize);

		PhysicalObject * obj = new PhysicalObject();
		//Use centre of the grid cell as the position
		glm::vec3 position = _offset + (index + glm::vec3(0.5f)) * _blockSize;
		obj->transform.translate(position);

		_grid[index.x][index.y][index.z] = obj;

		currentPos += increment;
	} while (glm::length(currentPos - end) > margin);
}

std::vector<Line> PhysicalObjectGenerator::GetOrderedTriangleLines(const Vertex &v1, const Vertex &v2, const Vertex &v3) const
{
	std::vector<Line> lines;
	lines.push_back(GetLine(v1, v2));
	lines.push_back(GetLine(v3, v1));
	lines.push_back(GetLine(v2, v3));

	std::sort(lines.begin(), lines.end(), [](Line &a, Line &b) { return a.length < b.length; });

	return lines;
}

Line PhysicalObjectGenerator::GetLine(const Vertex &v1, const Vertex &v2) const
{
	glm::vec3 direction = v2.Position - v1.Position;
	float length = glm::length(direction);

	Line line
	{
		v1.Position,
		v2.Position,
		glm::normalize(direction),
		length
	};

	return line;
}