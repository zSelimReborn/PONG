#include "Ball.h"

#include "Game.h"
#include "pk/Common.h"

Ball::Ball(const Transform& _Transform, const glm::vec3& _Direction, const float _Speed)
	: GameActor(_Transform), Direction(_Direction), BaseSpeed(_Speed), Speed(_Speed), SpeedIncrement(_Speed / 10.f), MaxSpeed(500.f)
{
}

Ball::Ball(const glm::vec3& _Location, const glm::vec3 _Size, const glm::vec3& _Direction, const float _Speed)
	: GameActor(_Location, _Size), Direction(_Direction), BaseSpeed(_Speed), Speed(_Speed), SpeedIncrement(_Speed / 10.f), MaxSpeed(500.f)
{
}

glm::vec3 Ball::GetDirection() const
{
	return Direction;
}

void Ball::SetSpeedIncrement(const float _Increment)
{
    SpeedIncrement = _Increment;
}

void Ball::SetMaxSpeed(const float _MaxSpeed)
{
    MaxSpeed = _MaxSpeed;
}

float Ball::GetSpeedIncrement() const
{
    return SpeedIncrement;
}

float Ball::GetMaxSpeed() const
{
    return MaxSpeed;
}

float Ball::GetSpeed() const
{
	return Speed;
}

void Ball::Bounce(const GameActor& Paddle)
{
    glm::vec3 Location = GetLocation();
    const glm::vec3 Difference = Location - Paddle.GetLocation();
    const float Distance = Difference.y;
    const float Percentage = Distance / Paddle.GetBoundingBox().ScaleOffset.y;

    Direction.x = -Direction.x;
    Direction.y = Percentage;

    // Adjust position after collision
    const float PaddleRight = Paddle.GetBoundingBox().Right();
    const float PaddleLeft = Paddle.GetBoundingBox().Left();

    float AdjustingAmount = (glm::sign(Direction.x) >= 1.0f) ? PaddleRight : PaddleLeft;
    AdjustingAmount += glm::sign(Direction.x) * GetSize().x;
    Location.x = AdjustingAmount;

    SetLocation(Location);
    IncrementSpeed();
}

void Ball::Update(const float Delta)
{
	GameActor::Update(Delta);

    bool bPlayerOneScored = false;

    const BoundingBox Box(GetBoundingBox());
    glm::vec3 Location(GetLocation());
    const float ScreenWidth = static_cast<float>(GetGame()->GetScreenWidth());
    const float ScreenHeight = static_cast<float>(GetGame()->GetScreenHeight());
    const glm::vec2 ScreenCenter = GetGame()->GetScreenCenter();

    if ((Location.x - Box.ScaleOffset.x) <= 0.f || (Location.x + Box.ScaleOffset.x) >= ScreenWidth)
    {
        if ((Location.x - Box.ScaleOffset.x) <= 0.f)
            bPlayerOneScored = false;
        if ((Location.x + Box.ScaleOffset.x) >= ScreenWidth)
            bPlayerOneScored = true;

        IncrementScore(bPlayerOneScored);
        Reset();

        const float x = (bPlayerOneScored) ? 1.f : -1.f;
        Direction = glm::vec3(x, 0.8f, 0.f);
        return;
    }

    // Position ball inside board and invert direction
    const glm::vec3 Middle(ScreenCenter.x, ScreenCenter.y, 1.0f);
    const glm::vec3 DirectionToCenter = glm::normalize(Middle - Location);
    if ((Location.y - Box.ScaleOffset.y) <= 0.f || (Location.y + Box.ScaleOffset.y) >= ScreenHeight)
    {
        Location.y += Box.ScaleOffset.y * glm::sign(DirectionToCenter.y);
        Direction.y = -Direction.y;
    }

    const glm::vec3 BallVelocity = (glm::normalize(Direction) * Speed) * Delta;
    Location += BallVelocity;

    SetLocation(Location);
}

void Ball::Reset()
{
    const glm::vec2 ScreenCenter = GetGame()->GetScreenCenter();

    SetLocation(glm::vec3(ScreenCenter.x, ScreenCenter.y, 1.f));
    Speed = BaseSpeed;
}

void Ball::IncrementScore(bool bPlayerOne) const
{
    GetGame()->IncrementScore(bPlayerOne);
}

void Ball::IncrementSpeed()
{
    Speed = Math::Clamp(Speed + SpeedIncrement, BaseSpeed, MaxSpeed);
}
