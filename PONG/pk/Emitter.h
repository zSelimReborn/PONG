#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>

#include "Shader.h"
#include "Texture.h"

struct Particle
{
	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec4 Color;
	float Life;
	float Speed;

	void Set(const glm::vec3& _Position, const glm::vec3& _Direction, const glm::vec4& _Color, const float _Life, const float Speed);
};

namespace ParticlePattern
{
	class Base
	{
	public:
		typedef std::shared_ptr<Base> SharedPtr;

		Base(bool _bLoop, const float _Speed, const float _Life, const int _SpawnAmount);

		bool ShouldLoop() const;
		float GetSpeed() const;
		float GetLife() const;
		int GetSpawnAmount() const;

		virtual void Spawn(std::vector<Particle*>& Pool, int& LastInactive, const glm::vec3& Position, const glm::vec3& Direction);

		virtual ~Base() = default;

	protected:
		static int NextInactive(std::vector<Particle*>& Pool, const int LastInactive);
		void SpawnParticle(Particle* NewParticle, const glm::vec3& Position, const glm::vec3& Direction) const;

	private:
		bool bLoop;
		float Speed;
		float Life;
		int SpawnAmount;
	};

	class Linear : public Base
	{
	public:
		Linear(const float _Speed, const float _Life, int _SpawnAmount);
		virtual void Spawn(std::vector<Particle*>& Pool, int& LastInactive, const glm::vec3& Position, const glm::vec3& Direction) override;

		virtual ~Linear() override = default;
	};

	class Bounce : public Base
	{
	public:
		Bounce(const float _Speed, const float _Life, int _SpawnAmount);
		virtual void Spawn(std::vector<Particle*>& Pool, int& LastInactive, const glm::vec3& Position, const glm::vec3& Direction) override;

		virtual ~Bounce() override = default;
	};
}

class Emitter
{
public:
	typedef std::unique_ptr<Emitter> UniquePtr;
	typedef std::shared_ptr<Emitter> SharedPtr;

	Emitter(const Shader::SharedPtr& _ParticleShader ,
		const Texture::SharedPtr& _ParticleTexture,
		int _PoolCapacity, const ParticlePattern::Base::SharedPtr& _ParticlePattern,
		const glm::mat4& _Projection
	);

	void Spawn(const glm::vec3& Position, const glm::vec3& Direction);
	void Update(const float Delta, const glm::vec3& Position, const glm::vec3& Direction);
	void Render() const;
	void Reset();

	void SetParticleScale(const float NewScale);
	float GetParticleScale() const;

	~Emitter();

private:
	void PrepareRenderQuad();

	void InitializePool();

	unsigned int QuadId;
	float ParticleScale;
	glm::mat4 RenderProjection;

	std::vector<Particle*> Pool;
	int LastInactive;
	int PoolCapacity;

	Shader::SharedPtr ParticleShader;
	Texture::SharedPtr ParticleTexture;

	ParticlePattern::Base::SharedPtr ParticlePattern;
};