#include "Game.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "pk/Common.h"
#include "pk/Font.h"

Game::Game(Window* _Window, const Transform& PlayerOneTransform, const Transform& PlayerTwoTransform,
           const float PlayerSpeed, const Transform& BallTransform, const glm::vec3& BallDirection, const float BallSpeed,
           const float BallSpeedIncrement, const float BallMaxSpeed)
		: PlayerOne(PlayerOneTransform, PlayerSpeed, GLFW_KEY_W, GLFW_KEY_S), PlayerTwo(PlayerTwoTransform, PlayerSpeed, GLFW_KEY_UP, GLFW_KEY_DOWN),
			Ball(BallTransform, BallDirection, BallSpeed),
			WindowPtr(_Window), Projection(0.f), PlayerOneScore(0), PlayerTwoScore(0)
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

	MainFont = std::make_unique<Font>("Fonts/Exan.ttf", "Exan", Projection);
	MainFont->Load(36);

	MainShader.Compile("main.vert", "main.frag");
	MainShader.Use();
	MainShader.SetMatrix("projection", Projection);

	OldTime = static_cast<float>(glfwGetTime());

	InitializeBricks();
	PlayerOne.Begin();
	PlayerTwo.Begin();
	Ball.Begin();
}

void Game::Frame()
{
	UpdateDelta();

	WindowPtr->ClearColor(Colors::BlackAlpha);
	WindowPtr->ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Update(Delta);
	RenderGame();

	WindowPtr->EndFrame();
}

bool Game::ShouldClose() const
{
	return WindowPtr->ShouldClose();
}

void Game::Update(const float Delta)
{
	Input(Delta);

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

	PlayerOne.Input(*WindowPtr, Delta);
	PlayerTwo.Input(*WindowPtr, Delta);
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
	MainShader.Use();

	Render(PlayerOne);
	Render(PlayerTwo);
	Render(Ball);
	for (const GameActor& Brick : Bricks)
	{
		Render(Brick);
	}

	RenderScore();
}

void Game::RenderScore() const
{
	std::string OneScore(std::to_string(PlayerOneScore));
	std::string TwoScore(std::to_string(PlayerTwoScore));

	glm::vec2 ScreenCenter(GetScreenCenter());

	const float* OneColor = (PlayerOneScore > PlayerTwoScore) ? Colors::Red : Colors::White;
	const float* SecondColor = (PlayerTwoScore > PlayerOneScore) ? Colors::Red : Colors::White;

	MainFont->Render(OneScore, glm::vec2(ScreenCenter.x - 100.f, 100.f), 1.0f, OneColor);
	MainFont->Render(TwoScore, glm::vec2(ScreenCenter.x + 100.f, 100.f), 1.0f, SecondColor);
}

void Game::Render(const GameActor& Actor) const
{
	MainShader.SetColor("spriteColor", Colors::White);

	glBindVertexArray(QuadId);
	MainShader.SetMatrix("model", Actor.GetRenderModel());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
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
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f,
		0.5f, -0.5f
	};

	unsigned int VAO = 0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
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
		Bricks.push_back(Brick);
	}
}
