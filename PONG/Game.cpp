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
#include "pk/AssetManager.h"
#include "Assets.h"

Game::Game(Window* _Window, const Transform& PlayerOneTransform, const Transform& PlayerTwoTransform,
           const float PlayerSpeed, const Transform& BallTransform, const glm::vec3& BallDirection, const float BallSpeed,
           const float BallSpeedIncrement, const float BallMaxSpeed, const int _WinScore)
		: PlayerOne(PlayerOneTransform, PlayerSpeed, GLFW_KEY_W, GLFW_KEY_S), PlayerTwo(PlayerTwoTransform, PlayerSpeed, GLFW_KEY_UP, GLFW_KEY_DOWN),
			Ball(BallTransform, BallDirection, BallSpeed),
			WindowPtr(_Window), Projection(0.f), PlayerOneScore(0), PlayerTwoScore(0), WinScore(_WinScore), State(GameState::MATCH)
{
	Projection = glm::ortho(0.f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()), 0.f, -1.0f, 1.0f);

	PlayerOne.SetGame(this);
	PlayerTwo.SetGame(this);
	Ball.SetGame(this);

	Ball.SetSpeedIncrement(BallSpeedIncrement);
	Ball.SetMaxSpeed(BallMaxSpeed);
}

void Game::Begin()
{
	LoadAssets();
	AssetManager& mAssetManager = AssetManager::Get();

	MainShader = mAssetManager.GetShader(Assets::MainShaderName);
	MainShader->Use();
	MainShader->SetMatrix("projection", Projection);

	MainFont = mAssetManager.GetFont(Assets::FontName);
	MainFont->Load(36);

	PlayerOne.SetTexture(mAssetManager.GetTexture(Assets::FirstPaddleSpriteName));
	PlayerTwo.SetTexture(mAssetManager.GetTexture(Assets::SecondPaddleSpriteName));
	Ball.SetTexture(mAssetManager.GetTexture(Assets::BallSpriteName));

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
	PlayerOne.Render();
	PlayerTwo.Render();

	if (State == GameState::MATCH)
	{
		Ball.Render();

		for (const GameActor& Brick : Bricks)
		{
			Brick.Render();
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
		SoundEngine::Get().Play(Assets::WinSound, 1.f);
	}
}

void Game::LoadAssets() const
{
	AssetManager& mAssetManager = AssetManager::Get();

	mAssetManager.LoadShader(Assets::ParticleShaderName, Assets::ParticleVertexShader, Assets::ParticleFragmentShader);
	mAssetManager.LoadShader(Assets::MainShaderName, Assets::MainVertexShader, Assets::MainFragmentShader);
	mAssetManager.LoadShader(Assets::TextShaderName, Assets::TextVertexShader, Assets::TextFragmentShader);
	mAssetManager.LoadFont(Assets::FontName, Assets::FontPath, Assets::TextShaderName, Projection);
	mAssetManager.LoadTexture(Assets::FirstPaddleSpriteName,
		Assets::FirstPaddleSprite,
		GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR
	);
	mAssetManager.LoadTexture(Assets::SecondPaddleSpriteName,
		Assets::SecondPaddleSprite,
		GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR
	);
	mAssetManager.LoadTexture(Assets::BallSpriteName,
		Assets::BallSprite,
		GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR
	);
	mAssetManager.LoadTexture(Assets::BrickSpriteName,
		Assets::BrickSprite,
		GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR
	);
}

void Game::UpdateDelta()
{
	CurrentTime = static_cast<float>(glfwGetTime());
	Delta = CurrentTime - OldTime;
	OldTime = CurrentTime;
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
		Brick.SetTexture(AssetManager::Get().GetTexture(Assets::BrickSpriteName));
		Bricks.push_back(Brick);
	}
}
