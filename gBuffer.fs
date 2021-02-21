#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Texture {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;

	bool useDiffuseMap;
	bool useNormalMap;
	bool useSpecularMap;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;
in mat3 TBN;

uniform Material material;
uniform Texture textureMap;

void main()
{    
	gPosition = fragPos;

	vec3 norm = normalize(normal);

	if(textureMap.useNormalMap) {
		norm = texture(textureMap.normal, texCoord).rgb;
        norm = normalize(norm * 2.0 - 1.0);  
		norm = normalize(TBN * norm); 
	}

	gNormal = norm;

	vec3 diffuse = vec3(material.diffuse);

	if(textureMap.useDiffuseMap) {
		diffuse *= texture(textureMap.diffuse, texCoord).rgb;
	}

	float specular = material.specular.r;

	if(textureMap.useSpecularMap) {
		specular *= texture(textureMap.specular, texCoord).r;
	}

	gAlbedoSpec.rgb = diffuse;
	gAlbedoSpec.a = specular;
 }