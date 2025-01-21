#include "Ball.h"

#include <iostream>

#include "Assets.h"
#include "Game.h"
#include "pk/AssetManager.h"
#include "pk/Common.h"
#include "pk/SoundEngine.h"

using namespace ParticlePattern;

Ball::Ball(const Transform& _Transform, const glm::vec3& _Direction, const float _Speed)
	: GameActor(_Transform), Direction(_Direction), BaseSpeed(_Speed), Speed(_Speed), SpeedIncrement(_Speed / 10.f), MaxSpeed(500.f)
{
    Initialize();
}

Ball::Ball(const glm::vec3& _Location, const glm::vec3 _Size, const glm::vec3& _Direction, const float _Speed)
	: GameActor(_Location, _Size), Direction(_Direction), BaseSpeed(_Speed), Speed(_Speed), SpeedIncrement(_Speed / 10.f), MaxSpeed(500.f)
{
    Initialize();
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
    PlayHitSound();

    glm::vec3 Location = GetLocation();
    const BoundingBox Box = Paddle.GetBoundingBox();

    const glm::vec3 Difference = Location - Paddle.GetLocation();
    const float Distance = Difference.y;
    const float Percentage = Distance / Box.ScaleOffset.y;

    Direction.x = -Direction.x;
    Direction.y = Percentage;

    // Adjust position after collision
    const float PaddleRight = Box.Right();
    const float PaddleLeft = Box.Left();

    float AdjustingAmount = (glm::sign(Direction.x) >= 1.0f) ? PaddleRight : PaddleLeft;
    AdjustingAmount += glm::sign(Direction.x) * (Box.ScaleOffset.x + 2.0f);
    Location.x = AdjustingAmount;

    SetLocation(Location);
    IncrementSpeed();

    BounceEmitter->Spawn(GetLocation(), glm::vec3(Direction.x, 0.f, 0.f));
}

void Ball::Begin()
{
	GameActor::Begin();

    constexpr float BounceParticleSpeed = 150.f;
    constexpr float BounceParticleLife = 0.8f;
    constexpr int BounceSpawnAmount = 3;
    constexpr int BouncePoolCapacity = BounceSpawnAmount * 4;
    constexpr float BounceParticleScale = 7.f;

    constexpr float TrailParticleSpeed = 0.1f;
    constexpr float TrailParticleLife = 0.5f;
    constexpr int TrailSpawnAmount = 2;
    constexpr int TrailEmitterPoolCapacity = 800;

    AssetManager& mAssetManager = AssetManager::Get();

    Base::SharedPtr LinearParticlePattern = std::make_shared<Linear>(TrailParticleSpeed, TrailParticleLife, TrailSpawnAmount);
    TrailEmitter = std::make_unique<Emitter>(mAssetManager.GetShader(Assets::ParticleShaderName), mAssetManager.GetTexture(Assets::BallSpriteName),
        TrailEmitterPoolCapacity, LinearParticlePattern, GetGame()->GetProjection()
    );

    Base::SharedPtr BouncePattern = std::make_shared<ParticlePattern::Bounce>(BounceParticleSpeed, BounceParticleLife, BounceSpawnAmount);
    BounceEmitter = std::make_unique<Emitter>(
        mAssetManager.GetShader(Assets::ParticleShaderName), mAssetManager.GetTexture(Assets::BallSpriteName), BouncePoolCapacity, BouncePattern, GetGame()->GetProjection()
    );
    BounceEmitter->SetParticleScale(BounceParticleScale);
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
        SoundEngine::Get().Play(Assets::GoalSound, 1.f);

        const float x = (bPlayerOneScored) ? 1.f : -1.f;
        Direction = glm::vec3(x, 0.8f, 0.f);
        return;
    }

    // Position ball inside board and invert direction
    const glm::vec3 Middle(ScreenCenter.x, ScreenCenter.y, 1.0f);
    const glm::vec3 DirectionToCenter = glm::normalize(Middle - Location);
    if ((Location.y - Box.ScaleOffset.y) <= 0.f || (Location.y + Box.ScaleOffset.y) >= ScreenHeight)
    {
        const float Base = (Location.y - Box.ScaleOffset.y) <= 0.f ? 0.f : ScreenHeight;
        Location.y = Base + (Box.ScaleOffset.y * glm::sign(DirectionToCenter.y));
        Direction.y = -Direction.y;

        PlayHitSound();
        BounceEmitter->Spawn(GetLocation(), glm::vec3(0.f, Direction.y, 0.f));
    }

    const glm::vec3 BallVelocity = (glm::normalize(Direction) * Speed) * Delta;
    Location += BallVelocity;

    SetLocation(Location);

    TrailEmitter->Update(Delta, GetLocation(), -Direction);
    BounceEmitter->Update(Delta, GetLocation(), Direction);
}

void Ball::Render() const
{
	GameActor::Render();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    TrailEmitter->Render();
    BounceEmitter->Render();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Ball::Initialize()
{
    SoundEngine::Get().Load(Assets::PongSound);
    SoundEngine::Get().Load(Assets::GoalSound);
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

void Ball::PlayHitSound()
{
    const int Channel = SoundEngine::Get().Play(Assets::PongSound, 0.05f);
}
