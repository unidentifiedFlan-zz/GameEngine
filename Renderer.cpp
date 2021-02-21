#include "Renderer.h"
#include <glm/glm.hpp>

namespace {
	float lerp(float a, float b, float f)
	{
		return (a * (1.0 - f)) + (b * f);
	}
}

Renderer::Renderer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenFramebuffers(1, &gBuffer);
	glGenTextures(1, &gPosition);
	glGenTextures(1, &gNormal);
	glGenTextures(1, &gAlbedoSpec);
	glGenRenderbuffers(1, &rboDepth);
	glGenFramebuffers(1, &ssaoFBO);
	glGenFramebuffers(1, &ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBuffer);
	glGenTextures(1, &ssaoColorBufferBlur);
	glGenTextures(1, &noiseTexture);
	generateNoiseTexture(noiseTexture);
	kernel = generateSampleKernel();
}

void Renderer::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::bindTexture( unsigned int textureID, unsigned int width, unsigned int height, GLenum attachment, GLint internalFormat, GLenum format, GLenum type )
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureID, 0);
}

void Renderer::bindDepthBuffer(unsigned int bufferID, unsigned int width, unsigned int height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, bufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bufferID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete!" << std::endl;
	}
}

void Renderer::renderLightingPass(Scene& scene, float aspectRatio)
{
	Shader lightingPass("DefaultLighting.vs", "DefaultLighting.fs");
	lightingPass.activate();
	lightingPass.setInt("gPosition", 0);
	lightingPass.setInt("gNormal", 1);
	lightingPass.setInt("gAlbedoSpec", 2);
	lightingPass.setInt("ssao", 3);
	lightingPass.setInt("useSSAO", true);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

	scene.registerLights(lightingPass, aspectRatio);
	renderQuad();
}

void Renderer::renderGeometryPass(Scene& scene, unsigned int fbo, unsigned int width, unsigned int height)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);


	bindTexture(gNormal, width, height, GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	bindTexture(gAlbedoSpec, width, height, GL_COLOR_ATTACHMENT2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	bindDepthBuffer(rboDepth, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float aspectRatio = (float)width / height;
	Shader geometryPassShader("gBuffer.vs", "gBuffer.fs");
	scene.drawGeometryPass(geometryPassShader, aspectRatio);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderForwardPass( Scene& scene, unsigned int fbo, unsigned int width, unsigned int height )
{
	// copy content of geometry's depth buffer to default framebuffer's depth buffer
// ----------------------------------------------------------------------------------
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
	// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
	// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader shader("LightSource.vs", "LightSource.fs");
	float aspectRatio = (float)width/ (float)height;
	scene.drawLights(shader, aspectRatio);
}

std::vector<glm::vec3> Renderer::generateSampleKernel()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	return ssaoKernel;
}

void Renderer::generateNoiseTexture(unsigned int textureID)
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);

	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

//One-dimensional only
void Renderer::blurTexture(unsigned int textureID, unsigned int width, unsigned int height)
{
	Shader blurShader("ssao.vs", "Blur.fs");
	blurShader.activate();
	blurShader.setInt("texInput", 0);

	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	bindTexture(ssaoColorBufferBlur, width, height, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED, GL_FLOAT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderSSAOPass(Scene& scene, unsigned int fbo, unsigned int width, unsigned height)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	bindTexture(ssaoColorBuffer, width, height, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED, GL_FLOAT);

	Shader SSAOShader("ssao.vs", "ssao.fs");
	SSAOShader.activate();
	SSAOShader.setInt("gPosition", 0);
	SSAOShader.setInt("gNormal", 1);
	SSAOShader.setInt("texNoise", 2);

	for (unsigned int i = 0; i < 64; ++i) {
		SSAOShader.setVec3("samples[" + std::to_string(i) + "]", kernel[i]);
    }

	float aspectRatio = (float)width / (float) height;
	glm::mat4 projection = glm::perspective(glm::radians(scene.camera()->zoom()), aspectRatio, 1.0f, 30.0f);
	SSAOShader.setMat4("projection", projection);
	glm::mat4 view = scene.camera()->getViewMatrix();
	SSAOShader.setMat4("view", view);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	blurTexture(ssaoColorBuffer, width, height);
}

void Renderer::render(Scene &scene, unsigned int width, unsigned int height)
{
	renderGeometryPass(scene, gBuffer, width, height);

	//LIGHTING
	float aspectRatio = (float)width / height;
	scene.drawShadowMap(aspectRatio);
	renderSSAOPass(scene, ssaoFBO, width, height);

	
//Shader debugDepthQuad("DebugTexture.vs", "DebugTexture.fs");
//debugDepthQuad.activate();
//debugDepthQuad.setInt("debugTex", 0);
//glActiveTexture(GL_TEXTURE0);
//glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
//renderQuad();

	renderLightingPass(scene, aspectRatio);
	renderForwardPass(scene, gBuffer, width, height);
}

Renderer::~Renderer()
{
}
