#pragma once
#include "RenderObject.h"
#include "EventHandler.h"

/*
* Implements Gerstner wave function
*/

struct DirectionalWave
{
	glm::vec3 direction;
	float frequency;
	float amplitude;
	float speed;
	float steepness;

	DirectionalWave(float iWavelength, float iSpeed, float iAmplitude, float iSteepness, glm::vec3 iDirection)
		:
		speed(iSpeed),
		amplitude(iAmplitude),
		steepness(iSteepness),
		direction(glm::normalize(iDirection))
	{
		frequency = sqrt(9.81f * 2.0f * glm::pi<float>() / iWavelength);
	}
};

class Liquid : public RenderObject
{

public:
	Liquid()
	{
		EventHandler<FrameUpdateEvent>::instance()->addListener(&Liquid::update, this);
	}

	void addWave(DirectionalWave &wave)
	{
		if (wave.frequency > 250.0) {
			_texWaves.push_back(wave);
		}
		else {
			_waves.push_back(wave);
		}
	}

	void update(FrameUpdateEvent e)
	{
		_time += e.numFrames;

		for (auto& vertex : _mesh->vertices) {
			Vertex originalVertex = vertex;
			vertex.Position.z = 0;
			vertex.Tangent = glm::vec3(0.0f, 1.0f, 0.0f);
			vertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f);

			for (auto& wave : _waves) {
				float phaseConstant = wave.speed * wave.frequency;

				float totalPhase = wave.frequency * glm::dot(wave.direction, originalVertex.Position) + phaseConstant * _time;
				float cosFactor = glm::cos(totalPhase);
				float sinFactor = glm::sin(totalPhase);

				vertex.Position.x += wave.steepness * wave.amplitude * wave.direction.x * cosFactor;
				vertex.Position.y += wave.steepness * wave.amplitude * wave.direction.y * cosFactor;
				vertex.Position.z += wave.amplitude * sinFactor;

				vertex.Tangent.x -= wave.steepness * wave.direction.x * wave.direction.y * wave.frequency * wave.amplitude * sinFactor;
				vertex.Tangent.y -= wave.steepness * wave.direction.y * wave.direction.y * wave.frequency * wave.amplitude * sinFactor;
				vertex.Tangent.z += wave.direction.y * wave.frequency * wave.amplitude * cosFactor;

				vertex.Normal.x -= wave.direction.x * wave.frequency * wave.amplitude * cosFactor;
				vertex.Normal.y -= wave.direction.y * wave.frequency * wave.amplitude * cosFactor;
				vertex.Normal.z -= wave.steepness * wave.frequency * wave.amplitude * sinFactor;
			}
		}
	}

	virtual void draw(Shader &shader, const Transform &worldTransform)
	{
		shader.setFloat("time", _time);
		shader.setInt("NUM_WAVES", _texWaves.size());

		for (int i = 0; i < _texWaves.size(); ++i) {
			std::string element = "waves[" + std::to_string(i) + "]";
			float phaseConstant = _texWaves[i].speed * _texWaves[i].frequency;
			float amplitudeFactor = _texWaves[i].amplitude*_texWaves[i].frequency;

			shader.setVec3(element + ".direction", _texWaves[i].direction);
			shader.setFloat(element + ".phase", phaseConstant);
			shader.setFloat(element + ".xAmplitude", _texWaves[i].direction.x*amplitudeFactor);
			shader.setFloat(element + ".yAmplitude", _texWaves[i].direction.y*amplitudeFactor);
		}

		RenderObject::draw(shader, worldTransform);
	}

	~Liquid()
	{
		EventHandler<FrameUpdateEvent>::instance()->removeListener(&Liquid::update, this);
	}

private:
	std::vector<DirectionalWave> _waves;
	std::vector<DirectionalWave> _texWaves;
	float _time;
};