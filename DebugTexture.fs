#version 330

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D debugTex;

void main()
{
	fragColor = vec4(texture(debugTex, TexCoords).rgb, 1.0);
}