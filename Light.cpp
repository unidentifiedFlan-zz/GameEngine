#include "Light.h"

Light::Light(Type type) : type(type) 
{
	Shader shader("shader.vs", "LightSource.fs");
	material.shader = shader;
}
