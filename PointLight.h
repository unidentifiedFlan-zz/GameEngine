#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();
	virtual void draw(glm::mat4 view, glm::mat4 projection, const Transform &worldTransform = Transform());

	float constant;
	float linear;
	float quadratic;
};