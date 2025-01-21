#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "Player.h"
#include "Ball.h"
#include "pk/Shader.h"

class Window;
class Font;
class Emitter;

enum class GameState : uint8_t
{
	MATCH,
	WIN,
	PAUSE
};

class Game
{
public:
	Game(Window* _Window, 
		const Transform& PlayerOneTransform, 
		const Transform& PlayerTwoTransform, 
		const float PlayerSpeed, 
		const Transform& BallTransform, 
		const glm::vec3& BallDirection,
		const float BallSpeed,
		const float BallSpeedIncrement,
		const float BallMaxSpeed,
		const int _WinScore
	);

	void Begin();
	void Frame();
	bool ShouldClose() const;

	int GetScreenWidth() const;
	int GetScreenHeight() const;
	glm::vec2 GetScreenCenter() const;

	glm::mat4 GetProjection() const;

	void IncrementScore(bool bPlayerOneScored);

private:
	void LoadAssets() const;
	void UpdateDelta();
	void Update(const float Delta);
	void Input(const float Delta);
	void CheckCollisions(const float Delta);

	void RenderGame() const;
	void RenderScore() const;
	void RenderWinScreen() const;
	void RenderBallParticles() const;
	void Render(const GameActor& Actor) const;

	void PrepareRenderQuad();

	void InitializeBricks();

	Player PlayerOne;
	Player PlayerTwo;
	Ball Ball;
	std::vector<GameActor> Bricks;
	std::shared_ptr<Emitter> BallEmitter;

	Window* WindowPtr;

	glm::mat4 Projection;
	std::shared_ptr<Shader> MainShader;
	unsigned int QuadId;

	float CurrentTime;
	float OldTime;
	float Delta;

	int PlayerOneScore;
	int PlayerTwoScore;
	int WinScore;

	std::shared_ptr<Font> MainFont;

	GameState State;
};

