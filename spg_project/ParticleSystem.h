#pragma once
#include "camera.h"
#include "Shader.h"


class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	
	void SetEmitterPosition(const glm::vec3& emitterPositionWorld);
	void SetEmitterDirection(const glm::vec3& emitterDirectionNormalized);
	void Update(const float& deltaTime, const float& gameTime, const camera& camera);
	void loadShaders();
private:
	void drawPassEmitter();
	void drawPass() const;
	void createVertexBuffers();
	

	struct ParticleShaderInput
	{
		glm::vec3 InitialPos;
		glm::vec3 InitialVel;
		float LifeTime;
		unsigned int Type;
	};

	Shader* particleShader;
	unsigned m_maxParticles = 0;
	bool m_isEmitterAlive = false;
	bool m_isEmitterPositionSet = false;
};
