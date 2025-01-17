#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>

class Shader;
class Texture;

struct Particle
{
	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec4 Color;
	float Life;
	float Speed;

	void Set(const glm::vec3& _Position, const glm::vec3& _Direction, const glm::vec4& _Color, const float _Life, const float Speed);
};

class Emitter
{
public:
	Emitter(const std::string& VertShader, const std::string& FragShader, 
		const std::string& TexturePath, 
		float _ParticleSpeed, float _ParticleLife, int _PoolCapacity, int _SpawnAmount, 
		const glm::mat4& _Projection
	);

	void Update(const float Delta, const glm::vec3& Position, const glm::vec3& Direction);
	void Render() const;
	void Reset();

	~Emitter();

private:
	void PrepareRenderQuad();

	void InitializePool();
	int NextInactive() const;
	void NewParticle(Particle* P, const glm::vec3& Position, const glm::vec3& Direction) const;

	unsigned int QuadId;
	glm::mat4 RenderProjection;

	std::vector<Particle*> Pool;
	int LastInactive;
	int PoolCapacity;
	int SpawnAmount;
	float ParticleSpeed;
	float ParticleLife;

	std::unique_ptr<Shader> ParticleShader;
	std::unique_ptr<Texture> ParticleTexture;
};