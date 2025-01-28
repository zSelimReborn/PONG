#include "Emitter.h"

#include "Shader.h"
#include "Texture.h"

#include <ctime>
#include <cstdlib>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "Common.h"

void Particle::Set(const glm::vec3& _Position, const glm::vec3& _Direction, const glm::vec4& _Color, const float _Life, const float _Speed)
{
	Position = _Position;
	Direction = _Direction;
	Color = _Color;
	Life = _Life;
	Speed = _Speed;
}

ParticlePattern::Base::Base(bool _bLoop, const float _Speed, const float _Life, const int _SpawnAmount)
	: bLoop(_bLoop), Speed(_Speed), Life(_Life), SpawnAmount(_SpawnAmount)
{
}

bool ParticlePattern::Base::ShouldLoop() const
{
	return bLoop;
}

float ParticlePattern::Base::GetSpeed() const
{
	return Speed;
}

float ParticlePattern::Base::GetLife() const
{
	return Life;
}

int ParticlePattern::Base::GetSpawnAmount() const
{
	return SpawnAmount;
}

void ParticlePattern::Base::Spawn(std::vector<Particle*>& Pool, int& LastInactive, const glm::vec3& Position, const glm::vec3& Direction)
{
}

int ParticlePattern::Base::NextInactive(std::vector<Particle*>& Pool, const int LastInactive)
{
	for (int i = LastInactive; i < Pool.size(); ++i)
	{
		if (Pool[i]->Life <= 0.f)
		{
			return i;
		}
	}

	for (int i = 0; i < LastInactive; ++i)
	{
		if (Pool[i]->Life <= 0.f)
		{
			return i;
		}
	}

	return 0;
}

void ParticlePattern::Base::SpawnParticle(Particle* NewParticle, const glm::vec3& Position, const glm::vec3& Direction) const
{
	if (!NewParticle)
	{
		return;
	}

	float RandomOffsetFactor = 0.f; //((rand() % 100) - 50) / 10.0f;
	float RandomColor = 0.5f + ((rand() % 100) / 100.0f);

	const glm::vec3 CalcPosition(Position.x + RandomOffsetFactor, Position.y + RandomOffsetFactor, 0.f);
	const glm::vec4 Color(RandomColor, RandomColor, RandomColor, 1.0f);

	NewParticle->Set(CalcPosition, Direction, Color, Life, Speed);
}

ParticlePattern::Linear::Linear(const float _Speed, const float _Life, int _SpawnAmount)
	: Base(true, _Speed, _Life, _SpawnAmount)
{
}

void ParticlePattern::Linear::Spawn(std::vector<Particle*>& Pool, int& LastInactive, const glm::vec3& Position,
	const glm::vec3& Direction)
{
	for (int i = 0; i < GetSpawnAmount(); ++i)
	{
		LastInactive = NextInactive(Pool, LastInactive);
		SpawnParticle(Pool[LastInactive], Position, Direction);
	}
}

ParticlePattern::Bounce::Bounce(const float _Speed, const float _Life, int _SpawnAmount)
	: Base(false, _Speed, _Life, _SpawnAmount)
{

}

void ParticlePattern::Bounce::Spawn(std::vector<Particle*>& Pool, int& LastInactive, const glm::vec3& Position,
	const glm::vec3& Direction)
{
	const std::vector<glm::vec3> Compass = {
		glm::vec3(0.f, -1.f, 0.f), // Top
		glm::vec3(1.f, 0.f, 0.f), // Right
		glm::vec3(0.f, 1.f, 0.f), // Bottom
		glm::vec3(-1.f, 0.f, 0.f), // Left
	};

	float MaxDot = -1.f;
	int DirectionIndex = 0;
	const glm::vec3 NormalizedDirection = glm::normalize(Direction);
	for (int i = 0; i < Compass.size(); ++i)
	{
		const float CurrentDot = glm::dot(NormalizedDirection, Compass[i]);
		if (CurrentDot > MaxDot)
		{
			MaxDot = CurrentDot;
			DirectionIndex = i;
		}
	}

	const glm::vec3 MainDirection = Compass[DirectionIndex];
	const glm::vec3 ToAdd = (MainDirection.x == 0.f) ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(0.f, 1.f, 0.f);

	std::vector<glm::vec3> DirectionsToSpawn;
	DirectionsToSpawn.push_back(MainDirection);
	DirectionsToSpawn.push_back(MainDirection + ToAdd);
	DirectionsToSpawn.push_back(MainDirection - ToAdd);

	const int DirectionsCount = DirectionsToSpawn.size();

	for (int i = 0; i < GetSpawnAmount(); ++i)
	{
		LastInactive = NextInactive(Pool, LastInactive);
		SpawnParticle(Pool[LastInactive], Position, DirectionsToSpawn[i % DirectionsCount]);
	}
}

Emitter::Emitter(const Shader::SharedPtr& _ParticleShader, const Texture::SharedPtr& _ParticleTexture,
	int _PoolCapacity, const ParticlePattern::Base::SharedPtr& _ParticlePattern,
	const glm::mat4& _Projection
)
	: QuadId(0), ParticleScale(5.0f),
		RenderProjection(_Projection), LastInactive(0), PoolCapacity(_PoolCapacity),
		ParticleShader(_ParticleShader), ParticleTexture(_ParticleTexture), ParticlePattern(_ParticlePattern)
{
	if (ParticleShader != nullptr)
	{
		ParticleShader->Use();
		ParticleShader->SetMatrix("projection", RenderProjection);
	}

	PrepareRenderQuad();
	InitializePool();

	srand(time(nullptr));
}

void Emitter::Spawn(const glm::vec3& Position, const glm::vec3& Direction)
{
	if (!ParticlePattern)
	{
		return;
	}

	ParticlePattern->Spawn(Pool, LastInactive, Position, Direction);
}

void Emitter::Update(const float Delta, const glm::vec3& Position, const glm::vec3& Direction)
{
	if (!ParticlePattern)
	{
		return;
	}

	if (ParticlePattern->ShouldLoop())
	{
		ParticlePattern->Spawn(Pool, LastInactive, Position, Direction);
	}

	const float ColorDecayFactor = 2.f / ParticlePattern->GetLife();
	for (Particle* CurrentParticle : Pool)
	{
		CurrentParticle->Life -= Delta;
		if (CurrentParticle->Life <= 0.f)
		{
			continue;
		}

		const glm::vec3 Velocity = CurrentParticle->Direction * CurrentParticle->Speed;
		CurrentParticle->Position += Velocity * Delta;
		CurrentParticle->Color.a -= ColorDecayFactor * Delta;
	}
}

void Emitter::Render() const
{
	ParticleShader->Use();

	ParticleShader->SetFloat("scale", ParticleScale);
	for (const Particle* CurrentParticle : Pool)
	{
		if (CurrentParticle->Life <= 0.f)
		{
			continue;
		}

		ParticleShader->SetFloat("position", CurrentParticle->Position);
		ParticleShader->SetFloat("color", CurrentParticle->Color);

		glBindVertexArray(QuadId);

		glActiveTexture(GL_TEXTURE0);
		ParticleTexture->Bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	ParticleTexture->UnBind();
}

void Emitter::Reset()
{
	for (Particle* mParticle : Pool)
	{
		mParticle->Life = 0.f;
	}

	LastInactive = 0;
}

void Emitter::SetParticleScale(const float NewScale)
{
	ParticleScale = NewScale;
}

float Emitter::GetParticleScale() const
{
	return ParticleScale;
}

Emitter::~Emitter()
{
	for (int i = 0; i < PoolCapacity; ++i)
	{
		delete Pool[i];
	}

	Pool.clear();
}

void Emitter::PrepareRenderQuad()
{
	float VertexData[] = {
		-0.5f, 0.5f, 0.f, 1.f,
		-0.5f, -0.5f, 0.f, 0.f,
		0.5f, -0.5f, 1.f, 0.f,
		-0.5f, 0.5f, 0.f, 1.f,
		0.5f, 0.5f, 1.f, 1.f,
		0.5f, -0.5f, 1.f, 0.f
	};

	unsigned int VAO = 0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	QuadId = VAO;
}

void Emitter::InitializePool()
{
	Pool.reserve(PoolCapacity);

	for (int i = 0; i < PoolCapacity; ++i)
	{
		Pool.push_back(new Particle());
	}
}

