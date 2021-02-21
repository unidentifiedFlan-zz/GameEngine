#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryShader) {

	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "Error: file not read." << std::endl;
	}

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode;


	if (geometryShader) {
		try {
			std::ifstream gShaderFile;
			gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			gShaderFile.open(geometryShader);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
			gShaderCode = geometryCode.c_str();

		}
		catch (std::ifstream::failure e) {
			std::cout << "Error: geometry shader not read" << std::endl;
		}
	}

	//COMPILE SHADERS
	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Error: vertex compilation failed\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Error: fragment compilation failed\n" << infoLog << std::endl;
	}

	if (geometryShader) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);

		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "Error: geometry compilation failed\n" << infoLog << std::endl;
		}
	}

	//Create Shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryShader) {
		glAttachShader(ID, geometry);
	}
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Error: Program linking failed\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryShader) {
		glDeleteShader(geometry);
	}
}

void Shader::activate()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 &matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setVec3(const std::string &name, float v0, float v1, float v2 ) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2);
}

void Shader::setVec3(const std::string &name, glm::vec3 &vector) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vector[0]);
}

void Shader::setVec4(const std::string &name, float v0, float v1, float v2, float v3) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2, v3);
}

void Shader::setVec4(const std::string &name, glm::vec4 &vector) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &vector[0]);
}