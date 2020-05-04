#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::SetEmitterPosition(const glm::vec3& emitterPositionWorld)
{
}

void ParticleSystem::SetEmitterDirection(const glm::vec3& emitterDirectionNormalized)
{
}

void ParticleSystem::Update(const float& deltaTime, const float& gameTime, const camera& camera)
{
}

void ParticleSystem::loadShaders()
{
}

void ParticleSystem::drawPassEmitter()
{
}

void ParticleSystem::drawPass() const
{
}

void ParticleSystem::createVertexBuffers()
{
}
