#pragma once
#include "Scene.h"
#include <random>

class Renderer
{
public:
	Renderer();
	void render(Scene& scene, unsigned int width, unsigned int height);
	~Renderer();
private:
	void renderQuad();
	void bindTexture(unsigned int textureID, unsigned int width, unsigned int height, GLenum attachment, GLint internalFormat, GLenum format, GLenum type );
	void bindDepthBuffer(unsigned int bufferID, unsigned int width, unsigned int height);
	void renderGeometryPass(Scene& scene, unsigned int fbo, unsigned int width, unsigned int height);
	void renderLightingPass(Scene& scene, float aspectRatio);
	void renderForwardPass(Scene& scene, unsigned int fbo, unsigned int width, unsigned int height);
	void renderSSAOPass(Scene& scene, unsigned int fbo, unsigned int width, unsigned height);
	std::vector<glm::vec3> generateSampleKernel();
	void generateNoiseTexture(unsigned int textureID);
	void blurTexture(unsigned int textureID, unsigned int width, unsigned int height);

	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec, rboDepth;
	unsigned int ssaoFBO, ssaoBlurFBO;
	unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
	unsigned int noiseTexture;
	std::default_random_engine generator;
	std::vector<glm::vec3> kernel;
};

