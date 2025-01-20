#include "Game.h"

#include <iostream>
#include <sstream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "pk/Window.h"
#include "pk/Common.h"
#include "pk/Font.h"
#include "pk/Emitter.h"
#include "pk/SoundEngine.h"
#include "Assets.h"

Game::Game(Window* _Window, const Transform& PlayerOneTransform, const Transform& PlayerTwoTransform,
           const float PlayerSpeed, const Transform& BallTransform, const glm::vec3& BallDirection, const float BallSpeed,
           const float BallSpeedIncrement, const float BallMaxSpeed, const int _WinScore)
		: PlayerOne(PlayerOneTransform, PlayerSpeed, GLFW_KEY_W, GLFW_KEY_S), PlayerTwo(PlayerTwoTransform, PlayerSpeed, GLFW_KEY_UP, GLFW_KEY_DOWN),
			Ball(BallTransform, BallDirection, BallSpeed),
			WindowPtr(_Window), Projection(0.f), PlayerOneScore(0), PlayerTwoScore(0), WinScore(_WinScore), State(GameState::MATCH)
{
	PlayerOne.SetGame(this);
	PlayerTwo.SetGame(this);
	Ball.SetGame(this);

	Ball.SetSpeedIncrement(BallSpeedIncrement);
	Ball.SetMaxSpeed(BallMaxSpeed);

	Projection = glm::ortho(0.f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()), 0.f, -1.0f, 1.0f);
}

void Game::Begin()
{
	PrepareRenderQuad();

	PlayerOne.SetTexture(Assets::FirstPaddleSprite);
	PlayerTwo.SetTexture(Assets::SecondPaddleSprite);
	Ball.SetTexture(Assets::BallSprite);

	MainFont = std::make_unique<Font>(Assets::FontPath, "Exan", Projection, Assets::TextVertexShader, Assets::TextFragmentShader);
	MainFont->Load(36);

	MainShader.Compile(Assets::MainVertexShader, Assets::MainFragmentShader);
	MainShader.Use();
	MainShader.SetMatrix("projection", Projection);

	BallEmitter = std::make_shared<Emitter>(Assets::ParticleVertexShader, Assets::ParticleFragmentShader, Assets::BallSprite, 
		0.1f, 0.5f, 800, 2, Projection
	);

	SoundEngine::Get().Load(Assets::WinSound);

	OldTime = static_cast<float>(glfwGetTime());

	InitializeBricks();
	PlayerOne.Begin();
	PlayerTwo.Begin();
	Ball.Begin();
}

void Game::Frame()
{
	UpdateDelta();
	SoundEngine::Get().Update(Delta);

	WindowPtr->ClearColor(Colors::LightBlack);
	WindowPtr->ClearFlags(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Input(Delta);

	if (State == GameState::MATCH)
	{
		Update(Delta);
	}

	RenderGame();

	if (State == GameState::WIN)
	{
		RenderWinScreen();
	}

	WindowPtr->CloseFrame();
}

bool Game::ShouldClose() const
{
	return WindowPtr->ShouldClose();
}

void Game::Update(const float Delta)
{
	PlayerOne.Update(Delta);
	PlayerTwo.Update(Delta);
	Ball.Update(Delta);

	BallEmitter->Update(Delta, Ball.GetLocation(), -Ball.GetDirection());

	CheckCollisions(Delta);
}

void Game::Input(const float Delta)
{
	if (WindowPtr->IsPressed(GLFW_KEY_ESCAPE))
	{
		WindowPtr->ShouldClose(true);
	}

	if (WindowPtr->IsPressed(GLFW_KEY_ENTER))
	{
		WindowPtr->Maximize();
	}

	if (WindowPtr->IsPressed(GLFW_KEY_SPACE) && State == GameState::WIN)
	{
		PlayerOneScore = 0;
		PlayerTwoScore = 0;
		State = GameState::MATCH;

		BallEmitter->Reset();
	}

	if (State == GameState::MATCH)
	{
		PlayerOne.Input(*WindowPtr, Delta);
		PlayerTwo.Input(*WindowPtr, Delta);
	}
}

void Game::CheckCollisions(const float Delta)
{
	if (Ball.Collide(PlayerOne))
	{
		Ball.Bounce(PlayerOne);
	}

	if (Ball.Collide(PlayerTwo))
	{
		Ball.Bounce(PlayerTwo);
	}
}

void Game::RenderGame() const
{
	RenderBallParticles();

	MainShader.Use();

	Render(PlayerOne);
	Render(PlayerTwo);

	if (State == GameState::MATCH)
	{
		Render(Ball);

		for (const GameActor& Brick : Bricks)
		{
			Render(Brick);
		}
	}

	RenderScore();
}

void Game::RenderScore() const
{
	const std::string OneScore(std::to_string(PlayerOneScore));
	const std::string TwoScore(std::to_string(PlayerTwoScore));

	const glm::vec2 ScreenCenter(GetScreenCenter());

	const float* OneColor = (PlayerOneScore > PlayerTwoScore) ? Colors::Green : Colors::White;
	const float* SecondColor = (PlayerTwoScore > PlayerOneScore) ? Colors::Green : Colors::White;

	MainFont->Render(OneScore, glm::vec2(ScreenCenter.x - 100.f, 100.f), 1.0f, OneColor);
	MainFont->Render(TwoScore, glm::vec2(ScreenCenter.x + 100.f, 100.f), 1.0f, SecondColor);
}

void Game::RenderWinScreen() const
{
	const std::string ReplayText = "Press SPACE to play again";
	const std::string Winner = (PlayerOneScore > PlayerTwoScore) ? "One" : "Two";
	std::ostringstream WinTextStream;
	WinTextStream << "Player " << Winner << " WON";
	const std::string WinText = WinTextStream.str();

	const glm::vec2 ScreenCenter(GetScreenCenter());
	MainFont->Render(WinText, glm::vec2(ScreenCenter.x - 170.f, ScreenCenter.y - 75.f), 1.0f, Colors::White);
	MainFont->Render(ReplayText, glm::vec2(ScreenCenter.x - 270.f, ScreenCenter.y - 25.f), 1.0f, Colors::White);
}

void Game::RenderBallParticles() const
{
	if (State == GameState::MATCH)
	{
		WindowPtr->SetBlendFunction(GL_SRC_ALPHA, GL_ONE);
		BallEmitter->Render();
		WindowPtr->SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void Game::Render(const GameActor& Actor) const
{
	MainShader.SetColor("spriteColor", Colors::White);

	glBindVertexArray(QuadId);
	MainShader.SetMatrix("model", Actor.GetRenderModel());

	glActiveTexture(GL_TEXTURE0);
	Actor.BindTexture();

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	Actor.UnBindTexture();
}

int Game::GetScreenWidth() const
{
	return WindowPtr->GetWidth();
}

int Game::GetScreenHeight() const
{
	return WindowPtr->GetHeight();
}

glm::vec2 Game::GetScreenCenter() const
{
	return WindowPtr->GetScreenCenter();
}

glm::mat4 Game::GetProjection() const
{
	return Projection;
}

void Game::IncrementScore(bool bPlayerOneScored)
{
	if (bPlayerOneScored)
	{
		PlayerOneScore++;
	}
	else
	{
		PlayerTwoScore++;
	}

	if (PlayerOneScore >= WinScore || PlayerTwoScore >= WinScore)
	{
		State = GameState::WIN;
		SoundEngine::Get().Play(Assets::WinSound);
	}
}

void Game::UpdateDelta()
{
	CurrentTime = static_cast<float>(glfwGetTime());
	Delta = CurrentTime - OldTime;
	OldTime = CurrentTime;
}

void Game::PrepareRenderQuad()
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

void Game::InitializeBricks()
{
	const glm::vec2 ScreenCenter = GetScreenCenter();
	const glm::vec3 BrickBasePos(ScreenCenter.x, 0.f, 0.f);
	const glm::vec3 BrickSize(5.f, 20.f, 1.f);
	const float BrickSpan = 30.f;

	int BrickQty = static_cast<int>(GetScreenHeight() / (BrickSize.y));

	for (int i = 0; i < BrickQty; ++i)
	{
		glm::vec3 CurrentBrickPos = BrickBasePos;
		CurrentBrickPos.y += (i * BrickSize.y) + (i * BrickSpan);

		GameActor Brick(CurrentBrickPos, BrickSize);
		Brick.SetTexture(Assets::BrickSprite);
		Bricks.push_back(Brick);
	}
}
