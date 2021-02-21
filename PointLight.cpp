#include "PointLight.h"

PointLight::PointLight() : Light(Type::Point) {}

void PointLight::draw(glm::mat4 view, glm::mat4 projection, const Transform &worldTransform)
{
	Shader shader = material.shader;
	shader.activate();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	Transform t = worldTransform * transform;
	glm::mat4 transformMatrix = t.transform();
	shader.setMat4("model", transformMatrix);
	shader.setVec3("lightColor", this->diffuse);
	_mesh->draw(shader);
}
