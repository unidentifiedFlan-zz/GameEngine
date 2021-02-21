#pragma once
#include <vector>
#include <list>
#include <string>
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"

struct FrameUpdateEvent
{
	int numFrames;
};

class RenderObject
{
public:
	void setMesh(const std::shared_ptr<Mesh>& mesh) { _mesh = mesh; }
	void addChild(const std::shared_ptr<RenderObject>& child)
	{ 
		_children.push_back(child);
	}
	virtual void draw(Shader &shader, const Transform &worldTransform = Transform());
	void unbindTextures();
	unsigned int numberOfTextures();

public:
	std::string name;
	Transform transform;
	Material  material;

protected:
	std::shared_ptr<Mesh> _mesh;
	std::vector<std::shared_ptr<RenderObject>> _children;

private:
	void setTexture(Texture &texture, Shader& shader);
};

