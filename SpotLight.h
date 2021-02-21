#pragma once
#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLight();

	glm::vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};