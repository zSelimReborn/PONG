#include "Player.h"

#include "pk/Window.h"

Player::Player(const Transform& _Transform, const float _Speed, const int _TopKey, const int _BottomKey)
	: GameActor(_Transform), TopKey(_TopKey), BottomKey(_BottomKey), Speed(_Speed)
{
}

Player::Player(const glm::vec3& _Location, const glm::vec3 _Size, const float _Speed, const int _TopKey, const int _BottomKey)
	: GameActor(_Location, _Size), TopKey(_TopKey), BottomKey(_BottomKey), Speed(_Speed)
{
}

void Player::SetSpeed(const float NewSpeed)
{
	Speed = NewSpeed;
}

float Player::GetSpeed() const
{
	return Speed;
}

void Player::Input(const Window& Window, const float Delta)
{
	GameActor::Input(Window, Delta);

	const BoundingBox Box(GetBoundingBox());
	glm::vec3 Location(GetLocation());
	const float WindowHeight = static_cast<float>(Window.GetHeight());

	const float Velocity = Speed * Delta;

	if (Window.IsPressed(TopKey))
	{
		if ((Location.y - Box.ScaleOffset.y) - Velocity > 0.f)
		{
			Location.y -= Velocity;
		}
	}

	if (Window.IsPressed(BottomKey))
	{
		if ((Location.y + Box.ScaleOffset.y) + Velocity < WindowHeight)
		{
			Location.y += Velocity;
		}
	}

	SetLocation(Location);
}
