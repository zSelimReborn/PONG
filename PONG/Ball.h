#pragma once

#include "GameActor.h"
#include "pk/Emitter.h"

class Ball : public GameActor
{
public:
	Ball(const Transform& _Transform, const glm::vec3& _Direction, const float _Speed);
	Ball(const glm::vec3& _Location, const glm::vec3 _Size, const glm::vec3& _Direction, const float _Speed);

	glm::vec3 GetDirection() const;

	void SetSpeedIncrement(const float _Increment);
	void SetMaxSpeed(const float _MaxSpeed);

	float GetSpeedIncrement() const;
	float GetMaxSpeed() const;
	float GetSpeed() const;

	void Bounce(const GameActor& Paddle);

	virtual void Begin() override;
	virtual void Update(const float Delta) override;
	virtual void Render() const override;

private:
	void Initialize();

	void Reset();
	void IncrementScore(bool bPlayerOne) const;
	void IncrementSpeed();

	void PlayHitSound();

	glm::vec3 Direction;
	float BaseSpeed;
	float Speed;
	float SpeedIncrement;
	float MaxSpeed;

	Emitter::UniquePtr TrailEmitter;
	Emitter::UniquePtr BounceEmitter;
};

