#pragma once
#include <string>

struct Texture {

	enum Type{
		UNASSIGNED = -1,
		DIFFUSE = 0,
		SPECULAR = 1,
		NORMAL = 2
	};

	unsigned int id;
	Type type;
	std::string path;

	Texture() : type(Type::UNASSIGNED) {}
};