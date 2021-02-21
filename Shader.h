#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>

#include <iostream>
#include <glad/glad.h> //OPENGL headers
#include <glm/glm.hpp>

class Shader
{
public:
	unsigned int ID;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr);

	void activate();

	void setBool(const std::string &name, bool value) const;

	void setInt(const std::string &name, int value) const;

	void setFloat(const std::string &name, float value) const;

	void setMat4(const std::string &name, glm::mat4 &matrix) const;

	void setVec3(const std::string &name, float v0, float v1, float v2) const;

	void setVec3(const std::string &name, glm::vec3 &vector) const;

	void setVec4(const std::string &name, float v0, float v1, float v2, float v4) const;

	void setVec4(const std::string &name, glm::vec4 &vector) const;

};

#endif