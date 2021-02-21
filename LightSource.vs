#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragPos = (model*vec4(aPos, 1.0)).xyz;
	//Efficiency: Move inverse calculation on CPU and transfer via uniform variable
	normal = mat3(transpose(inverse(model))) * aNormal;  
	texCoord = aTexCoord;
	gl_Position = projection * view * vec4(fragPos, 1.0f);
}