#pragma once

#include "GameActor.h"
class Player : public GameActor
{
public:
	Player(const Transform& _Transform, const float _Speed, const int _TopKey, const int _BottomKey);
	Player(const glm::vec3& _Location, const glm::vec3 _Size, const float _Speed, const int _TopKey, const int _BottomKey);

	void SetSpeed(const float NewSpeed);
	float GetSpeed() const;

	virtual void Input(const Window& Window, const float Delta) override;

private:
	int TopKey;
	int BottomKey;
	float Speed;
};

