#version 460 core
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

#define MAX_DIRECTIONAL_LIGHTS 4
struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define MAX_POINT_LIGHTS 4
struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
	bool deactivated;
};

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;
in mat3 TBN;
in vec4 FragPosLightSpace[MAX_DIRECTIONAL_LIGHTS];

uniform int NUM_DIRECTIONAL_LIGHTS;
uniform DirLight directionalLight[MAX_DIRECTIONAL_LIGHTS];
uniform SpotLight spotLight;
uniform int NUM_POINT_LIGHTS;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform vec3 viewPos;
uniform Material material;
uniform Texture textureMap;
uniform sampler2DArray shadowDirectionalMap;
uniform samplerCubeArray shadowCubeMap;
uniform float far_plane;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)

);

float DirectionalShadowCalculation(int lightIndex, vec3 norm)
{
	vec4 fragPosLightSpace = FragPosLightSpace[lightIndex];
	DirLight light = directionalLight[lightIndex];
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.005 * (1.0 - dot(norm, light.direction)), 0.0005);
	
	float shadow = 0.0;
	vec3 texelSize = 1.0 / textureSize(shadowDirectionalMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowDirectionalMap, vec3(projCoords.xy + vec2(x, y)* texelSize.xy, lightIndex)).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0; 

	if(projCoords.z > 1.0) {
        shadow = 0.0;
	}

    return shadow;
}

vec3 CalcDirLight(int lightIndex, vec3 norm, vec3 viewDir)
{
    DirLight light = directionalLight[lightIndex];
	vec3 lightDir = normalize(-light.direction);

	vec3 ambient = vec3(light.ambient * material.ambient);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = vec3(diff*light.diffuse* material.diffuse);
	
	if(textureMap.useDiffuseMap) {
		ambient *= texture(textureMap.diffuse, texCoord).rgb;
		diffuse *= texture(textureMap.diffuse, texCoord).rgb;
	}

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = vec3(material.specular * spec * light.specular);

	if(textureMap.useSpecularMap) {
		specular *= texture(textureMap.specular, texCoord).rgb;
	}

	float shadow = DirectionalShadowCalculation(lightIndex, norm);     

	return (ambient + (1.0-shadow)*(diffuse + specular));
}

float PointShadowCalculation(vec3 fragPos, vec3 lightPos, int shadowIndex)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;

    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowCubeMap, vec4(fragToLight + gridSamplingDisk[i] * diskRadius, shadowIndex)).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    //DEBUG
        
    return shadow;
}


vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, int shadowIndex)
{
	vec3 lightDir = normalize(light.position - fragPos);

	vec3 ambient = vec3(light.ambient * material.ambient);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = vec3(diff*light.diffuse* material.diffuse);

	if(textureMap.useDiffuseMap) {
		ambient *= texture(textureMap.diffuse, texCoord).rgb;
		diffuse *= texture(textureMap.diffuse, texCoord).rgb;
	}

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = vec3(material.specular * spec * light.specular);

	if(textureMap.useSpecularMap) {
		specular *= texture(textureMap.specular, texCoord).rgb;
	}

	float distance = length(light.position.xyz - fragPos);
	float attenuation = 1.0/(light.constant + light.linear*distance
	                         + light.quadratic*(distance*distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float visibility = step(0.0, dot(viewDir, norm));

	float shadow = PointShadowCalculation(fragPos, light.position, shadowIndex);     

	return visibility*(ambient + (1.0-shadow)*(diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir )
{
	if(light.deactivated) {
		return vec3(0.0);
	}

	vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = vec3(light.ambient);
    vec3 diffuse = vec3(light.diffuse * diff);

	if(textureMap.useDiffuseMap) {
		ambient *= texture(textureMap.diffuse, texCoord).rgb;
		diffuse *= texture(textureMap.diffuse, texCoord).rgb;
	}

    vec3 specular = vec3(light.specular * spec); //No material spec?
	if(textureMap.useSpecularMap) {
		specular *= texture(textureMap.specular, texCoord).rgb;
	}

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{    
	vec3 result = vec3(0.0);

	vec3 norm = normalize(normal);

	if(textureMap.useNormalMap) {
		norm = texture(textureMap.normal, texCoord).rgb;
        norm = normalize(norm * 2.0 - 1.0);  
		norm = normalize(TBN * norm); 
	}

	vec3 viewDir = normalize(viewPos - fragPos);
	
	for(int i = 0; i < NUM_DIRECTIONAL_LIGHTS; ++i) {
		result += CalcDirLight(i, norm, viewDir);
	}
	result += CalcSpotLight(spotLight, norm, fragPos, viewDir);

	for(int i = 0; i < NUM_POINT_LIGHTS; ++i) {
		result += CalcPointLight(pointLights[i], norm, fragPos, viewDir, i);
	}
    
	FragColor = vec4(result, 1.0);
}