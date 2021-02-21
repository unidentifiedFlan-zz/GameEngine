#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;
out mat3 TBN;
#define MAX_DIRECTIONAL_LIGHTS 4
out vec4 FragPosLightSpace[MAX_DIRECTIONAL_LIGHTS];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrices[MAX_DIRECTIONAL_LIGHTS];
uniform int  NUM_DIRECTIONAL_LIGHTS;

void main()
{
	fragPos = vec3(model*vec4(aPos, 1.0));
	//Efficiency: Move inverse calculation on CPU and transfer via uniform variable
	normal = mat3(transpose(inverse(model))) * aNormal;  
	texCoord = aTexCoord;
	gl_Position = projection * view * vec4(fragPos, 1.0f);

    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
	vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
	for(int i = 0; i < NUM_DIRECTIONAL_LIGHTS; ++i) {
		FragPosLightSpace[i] = lightSpaceMatrices[i] * vec4(fragPos, 1.0);
	}
}