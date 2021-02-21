#include "RenderObject.h"

void RenderObject::setTexture(Texture &texture, Shader& shader)
{
	if (texture.type == Texture::Type::UNASSIGNED) {
		return;
	}

	glActiveTexture(GL_TEXTURE0 + 0x0001 * texture.type);
	std::string name;
	if (texture.type == Texture::Type::DIFFUSE) {
		name = "textureMap.diffuse";
		shader.setBool("textureMap.useDiffuseMap", true);
	}
	else if (texture.type == Texture::Type::SPECULAR) {
		name = "textureMap.specular";
		shader.setBool("textureMap.useSpecularMap", true);
	}
	else if (texture.type == Texture::Type::NORMAL) {
		name = "textureMap.normal";
		shader.setBool("textureMap.useNormalMap", true);
	}
	shader.setInt((name).c_str(), texture.type);

	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void RenderObject::draw(Shader &shader, const Transform &worldTransform)
{
	shader.activate();

	Transform t = worldTransform*transform;
	glm::mat4 transformMatrix = t.transform();
	shader.setMat4("model", transformMatrix);

	shader.setInt("textureMap.useNormalMap", false);
	shader.setInt("textureMap.useDiffuseMap", false);
	shader.setInt("textureMap.useSpecularMap", false);
	shader.setVec3("material.ambient", material.ambient);
	shader.setVec3("material.diffuse", material.diffuse);
	shader.setVec3("material.specular", material.specular);
	shader.setFloat("material.shininess", material.shininess);

	setTexture(material.diffuseMap, shader);
	setTexture(material.specularMap, shader);
	setTexture(material.normalMap, shader);

	if (_mesh) {
		_mesh->draw(shader);
	}

	unbindTextures();

	for (auto& child : _children) {
		child->draw(shader, t);
	}
}

void RenderObject::unbindTextures()
{
	glActiveTexture(GL_TEXTURE0 + 0x0001 * Texture::Type::DIFFUSE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0 + 0x0001 * Texture::Type::NORMAL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0 + 0x0001 * Texture::Type::SPECULAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int RenderObject::numberOfTextures()
{
	unsigned int numTex = 0;
	numTex += material.normalMap.type != Texture::Type::UNASSIGNED;
	numTex += material.specularMap.type != Texture::Type::UNASSIGNED;
	numTex += material.diffuseMap.type != Texture::Type::UNASSIGNED;

	for (const auto& obj : _children) {
		numTex += obj->numberOfTextures();
	}

	return numTex;
}
