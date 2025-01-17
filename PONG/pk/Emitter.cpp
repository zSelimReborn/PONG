#include "Emitter.h"

#include "Shader.h"
#include "Texture.h"

#include <ctime>
#include <cstdlib>
#include <iostream>

#include "Common.h"

void Particle::Set(const glm::vec3& _Position, const glm::vec3& _Direction, const glm::vec4& _Color, const float _Life, const float _Speed)
{
	Position = _Position;
	Direction = _Direction;
	Color = _Color;
	Life = _Life;
	Speed = _Speed;
}

Emitter::Emitter(const std::string& VertShader, const std::string& FragShader, const std::string& TexturePath, 
	float _ParticleSpeed, float _ParticleLife, int _PoolCapacity, int _SpawnAmount, 
	const glm::mat4& _Projection
)
	: QuadId(0), LastInactive(0),
		PoolCapacity(_PoolCapacity), SpawnAmount(_SpawnAmount),
		ParticleSpeed(_ParticleSpeed), ParticleLife(_ParticleLife),
		RenderProjection(_Projection)
{
	ParticleShader = std::make_unique<Shader>();
	ParticleShader->Compile(VertShader, FragShader);
	ParticleShader->Use();
	ParticleShader->SetMatrix("projection", RenderProjection);

	ParticleTexture = std::make_unique<Texture>(TexturePath, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	PrepareRenderQuad();
	InitializePool();

	srand(time(nullptr));
}

void Emitter::Update(const float Delta, const glm::vec3& Position, const glm::vec3& Direction)
{
	for (int i = 0; i < SpawnAmount; ++i)
	{
		LastInactive = NextInactive();
		NewParticle(Pool[LastInactive], Position, Direction);
	}

	const float ColorDecayFactor = 2.f / ParticleLife;
	for (Particle* P : Pool)
	{
		P->Life -= Delta;
		if (P->Life <= 0.f)
		{
			continue;
		}

		const glm::vec3 Velocity = P->Direction * P->Speed;
		P->Position += Velocity * Delta;
		P->Color.a -= ColorDecayFactor * Delta;
	}
}

void Emitter::Render() const
{
	ParticleShader->Use();

	for (const Particle* P : Pool)
	{
		if (P->Life <= 0.f)
		{
			continue;
		}

		ParticleShader->SetFloat("position", P->Position);
		ParticleShader->SetFloat("color", P->Color);

		glBindVertexArray(QuadId);

		glActiveTexture(GL_TEXTURE0);
		ParticleTexture->Bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	ParticleTexture->UnBind();
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

int Emitter::NextInactive() const
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

void Emitter::NewParticle(Particle* P, const glm::vec3& Position, const glm::vec3& Direction) const
{
	if (!P)
	{
		return;
	}

	float RandomOffsetFactor = 0.f; //((rand() % 100) - 50) / 10.0f;
	float RandomColor = 0.5f + ((rand() % 100) / 100.0f);

	const glm::vec3 CalcPosition(Position.x + RandomOffsetFactor, Position.y + RandomOffsetFactor, 0.f);
	const glm::vec4 Color(RandomColor, RandomColor, RandomColor, 1.0f);

	P->Set(CalcPosition, Direction, Color, ParticleLife, ParticleSpeed);
}
