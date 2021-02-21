#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=72) out; //MAX_POINT_LIGHTS*18

#define MAX_POINT_LIGHTS 4

uniform mat4 shadowMatrices[6*MAX_POINT_LIGHTS];
uniform vec3 lightPositions[MAX_POINT_LIGHTS];
uniform int  NUM_POINT_LIGHTS;

out vec4 FragPos; // FragPos from GS (output per emitvertex)
out vec3 LightPos;

void main()
{
	for(int lightInd = 0; lightInd < NUM_POINT_LIGHTS; ++lightInd)
	{
		for(int face = 0; face < 6; ++face)
		{
			gl_Layer = lightInd*6 + face; // built-in variable that specifies to which face we render.
			for(int i = 0; i < 3; ++i) // for each triangle's vertices
			{
				FragPos = gl_in[i].gl_Position;
				LightPos = lightPositions[lightInd];
				gl_Position = shadowMatrices[gl_Layer] * FragPos;
				EmitVertex();
			}

			EndPrimitive();
		}
	}
} 