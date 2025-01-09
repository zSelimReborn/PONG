#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#include "Window.h"
#include "pk/Common.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

void ProcessInput(const Window& window, const float Delta);

unsigned int PrepareRectangle();

glm::vec3 CheckBallCollision(const glm::vec3& PaddlePos, const glm::vec3& PaddleSize, glm::vec3& BallPosition, const glm::vec3& BallSize, const glm::vec3& BallDirection, float& BallSpeed);
void UpdateBall(glm::vec3& Position, const glm::vec3& Size, glm::vec3& CurrentDirection, int& OneScore, int& TwoScore);
glm::vec3 CalculateNormal(const glm::vec3& Direction);

Shader MainShader;

constexpr float MiddleScreenX = WINDOW_WIDTH / 2;
constexpr float MiddleScreenY = WINDOW_HEIGHT / 2;

constexpr float PlayerSpeed = 300.f;
glm::vec3 PlayerOnePos(MiddleScreenX - 350.f, WINDOW_HEIGHT / 2, 0.f);
glm::vec3 PlayerTwoPos(MiddleScreenX + 350.f, WINDOW_HEIGHT / 2, 0.f);
glm::vec3 PlayerSize(6.5f, 50.f, 1.f);

glm::vec3 BrickBasePos(MiddleScreenX, 0.f, 0.f);
glm::vec3 BrickSize(5.f, 20.f, 1.f);
float BrickSpan = 30.f;
int BrickQty = static_cast<int>(WINDOW_HEIGHT / (BrickSize.y));

constexpr float BallBaseSpeed = 200.f;
constexpr float BallMaxSpeed = 300.f;
constexpr float BallSpeedStep = 5.f; 
float BallSpeed = BallBaseSpeed;
glm::vec3 BallBasePos(MiddleScreenX, MiddleScreenY, 1.f);
glm::vec3 BallSize(10.f, 12.f, 1.f);
glm::vec3 BallDirection(1.f, 0.5f, 0.f);

int PlayerOneScore = 0, PlayerTwoScore = 0;

int main(int argc, char** argv)
{
    Window w(WINDOW_WIDTH, WINDOW_HEIGHT, "PONG");

    try
    {
        w.Initialize();
        w.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } catch (const Window::Error& Error)
    {
        std::cout << Error.what() << "\n";
        glfwTerminate();
        return -1;
    }

    try
    {
        MainShader.Compile("main.vert", "main.frag");
    } catch (const Shader::ShaderCompileError& error)
    {
        std::cout << "ERROR: UNABLE TO COMPILE SHADER: " << error.what() << "\n";
        glfwTerminate();
        return -1;
    }

    glm::mat4 Ortho = glm::ortho(0.f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.f, -1.0f, 1.0f);

    MainShader.Use();
    MainShader.SetMatrix("projection", Ortho);

    glm::mat4 Identity(1.0f);

    unsigned int Quad = PrepareRectangle();

    float OldTime = static_cast<float>(glfwGetTime());
    float CurrentTime = OldTime;
    float Delta = CurrentTime - OldTime;

    // Render loop
    while (!w.ShouldClose())
    {
        CurrentTime = static_cast<float>(glfwGetTime());
        Delta = CurrentTime - OldTime;
        OldTime = CurrentTime;

        w.ClearColor(Colors::BlackAlpha);
        w.ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ProcessInput(w, Delta);

        MainShader.SetColor("spriteColor", Colors::White);

        glm::mat4 PlayerOneModel = glm::translate(Identity, PlayerOnePos);
        glm::mat4 PlayerTwoModel = glm::translate(Identity, PlayerTwoPos);
        PlayerOneModel = glm::scale(PlayerOneModel, PlayerSize);
        PlayerTwoModel = glm::scale(PlayerTwoModel, PlayerSize);

        UpdateBall(BallBasePos, BallSize, BallDirection, PlayerOneScore, PlayerTwoScore);
        BallDirection = CheckBallCollision(PlayerOnePos, PlayerSize, BallBasePos, BallSize, BallDirection, BallSpeed);
        BallDirection = CheckBallCollision(PlayerTwoPos, PlayerSize, BallBasePos, BallSize, BallDirection, BallSpeed);

        const glm::vec3 BallVelocity = (glm::normalize(BallDirection) * BallSpeed) * Delta;
        BallBasePos += BallVelocity;
        glm::mat4 BallModel = glm::translate(Identity, BallBasePos);
        BallModel = glm::scale(BallModel, BallSize);

        glBindVertexArray(Quad);

        MainShader.SetMatrix("model", PlayerOneModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        MainShader.SetMatrix("model", PlayerTwoModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        MainShader.SetMatrix("model", BallModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        for (int i = 0; i < BrickQty; ++i)
        {
            glm::vec3 CurrentBrickPos = BrickBasePos;
            CurrentBrickPos.y += (i * BrickSize.y) + (i * BrickSpan);

            glm::mat4 BrickModel = glm::translate(Identity, CurrentBrickPos);
            BrickModel = glm::scale(BrickModel, BrickSize);

            MainShader.SetMatrix("model", BrickModel);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        //std::cout << "P1: " << PlayerOneScore << " | P2: " << PlayerTwoScore << "\n";
        w.EndFrame();
    }

    glfwTerminate();
    return 0;
}

void ProcessInput(const Window& window, const float Delta)
{
    if (window.IsPressed(GLFW_KEY_ESCAPE))
    {
        window.ShouldClose(true);
    }

    const float PlayerMovement = PlayerSpeed * Delta;
    const glm::vec2 PlayerPaddleSizeOffset(PlayerSize.x / 2, PlayerSize.y / 2);

    if (window.IsPressed(GLFW_KEY_W))
    {
        if ((PlayerOnePos.y - PlayerPaddleSizeOffset.y) - PlayerMovement > 0.f)
        {
            PlayerOnePos.y -= PlayerMovement;
        }
    }

    if (window.IsPressed(GLFW_KEY_S))
    {
        if ((PlayerOnePos.y + PlayerPaddleSizeOffset.y) + PlayerMovement < WINDOW_HEIGHT)
        {
            PlayerOnePos.y += PlayerMovement;
        }
    }

    if (window.IsPressed(GLFW_KEY_UP))
    {
        if ((PlayerTwoPos.y - PlayerPaddleSizeOffset.y) - PlayerMovement > 0.f)
        {
            PlayerTwoPos.y -= PlayerMovement;
        }
    }

    if (window.IsPressed(GLFW_KEY_DOWN))
    {
        if ((PlayerTwoPos.y + PlayerPaddleSizeOffset.y) + PlayerMovement < WINDOW_HEIGHT)
        {
            PlayerTwoPos.y += PlayerMovement;
        }
    }
}

unsigned int PrepareRectangle()
{
    /* float rectangle[] = {
        -0.5f, 0.7f,
        -0.5f, -0.7f,
        0.5f, -0.7f,
		-0.5f, 0.7f,
        0.5f, 0.7f,
        0.5f, -0.7f
    }; */

    float rectangle[] = {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

glm::vec3 CheckBallCollision(const glm::vec3& PaddlePos, const glm::vec3& PaddleSize, glm::vec3& BallPosition,
	const glm::vec3& BallSize, const glm::vec3& BallDirection, float& BallSpeed)
{
    glm::vec3 NewDirection = BallDirection;
    glm::vec2 PaddleSizeOffset(PaddleSize.x / 2.0f, PaddleSize.y / 2.0f);
    glm::vec2 BallSizeOffset(BallSize.x / 2.0f, BallSize.y / 2.0f);

    const float PaddleRight = PaddlePos.x + PaddleSizeOffset.x;
    const float PaddleLeft = PaddlePos.x - PaddleSizeOffset.x;
    const float PaddleTop = PaddlePos.y + PaddleSizeOffset.y;
    const float PaddleBottom = PaddlePos.y - PaddleSizeOffset.y;

    const float BallRight = BallPosition.x + BallSizeOffset.x;
    const float BallLeft = BallPosition.x - BallSizeOffset.x;
    const float BallTop = BallPosition.y + BallSizeOffset.y;
    const float BallBottom = BallPosition.y - BallSizeOffset.y;

	bool bCollidedX = (BallRight >= PaddleLeft && PaddleRight >= BallLeft);
    bool bCollidedY = (BallBottom <= PaddleTop && PaddleBottom <= BallTop);

    if (bCollidedX && bCollidedY)
    {
        /* glm::vec3 Left(-1.0f, 0.f, 0.f);
        glm::vec3 Right(-1.0f, 0.f, 0.f);

        glm::vec3 Normal = (glm::sign(BallDirection.x) >= 1.0f) ? Left : Right;
        NewDirection = glm::reflect(glm::normalize(NewDirection), Normal); */

        const glm::vec3 Difference = BallPosition - PaddlePos;
        const float Distance = Difference.y;
        const float Percentage = Distance / PaddleSizeOffset.y;

        NewDirection.x = -NewDirection.x;
        NewDirection.y = Percentage;

        float adjusted = (glm::sign(NewDirection.x) >= 1.0f) ? PaddleRight : PaddleLeft;
        adjusted += glm::sign(NewDirection.x) * BallSize.x;
        BallPosition.x = adjusted;

        BallSpeed = Math::Clamp(BallSpeed + BallSpeedStep, BallBaseSpeed, BallMaxSpeed);
    }

    return NewDirection;
}

void UpdateBall(glm::vec3& Position, const glm::vec3& Size, glm::vec3& CurrentDirection, int& OneScore, int& TwoScore)
{
    bool bScored = false;

    glm::vec2 SizeOffset(Size.x / 2, Size.y / 2);

    if ((Position.x - SizeOffset.x) <= 0.f || (Position.x + SizeOffset.x) >= WINDOW_WIDTH)
    {
        if ((Position.x - SizeOffset.x) <= 0.f)
            TwoScore++;
        if ((Position.x + SizeOffset.x) >= WINDOW_WIDTH)
            OneScore++;

        Position = glm::vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 1.f);
        BallSpeed = BallBaseSpeed;

        const float x = (OneScore > TwoScore) ? 1.f : -1.f;
        CurrentDirection = glm::vec3(x, 0.8f, 0.f);
    }

    // Position ball inside board and invert direction
    const glm::vec3 Middle(MiddleScreenX, MiddleScreenY, 1.f);
    const glm::vec3 Direction = glm::normalize(Middle - Position);
    if ((Position.y - SizeOffset.y) <= 0.f || (Position.y + SizeOffset.y) >= WINDOW_HEIGHT)
    {
        Position.y += SizeOffset.y * glm::sign(Direction.y);
        CurrentDirection.y = -CurrentDirection.y;
    }
}

glm::vec3 CalculateNormal(const glm::vec3& Target)
{
    const glm::vec3 Compass[] = {
        glm::vec3(-1.0f, 0.f, 0.f),
        glm::vec3(-1.0f, 1.0f, 0.f),
        glm::vec3(0.f, 1.0f, 0.f),
        glm::vec3(1.0f, 1.0f, 0.f),
        glm::vec3(1.0f, 0.0f, 0.f),
        glm::vec3(1.0f, -1.0f, 0.f),
        glm::vec3(0.f, -1.0f, 0.f),
        glm::vec3(-1.0f, -1.0f, 0.f),
    };

    float MaxDot = -1.f;
    glm::vec3 Normal = Compass[0];
    const glm::vec3 TargetNormalized = glm::normalize(Target);
    for (const glm::vec3& Direction : Compass)
    {
        const float CurrentDot = glm::dot(Direction, TargetNormalized);
	    if (CurrentDot >= MaxDot)
	    {
            MaxDot = CurrentDot;
            Normal = Direction;
	    }
    }

    return Normal;
}
