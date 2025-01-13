#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Game.h"
#include "Shader.h"
#include "Window.h"
#include "GameActor.h"
#include "pk/Font.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr float PLAYER_SPEED = 300.f;

constexpr float BALL_BASE_SPEED = 200.f;
constexpr float BALL_MAX_SPEED = 500.f;
constexpr float BALL_SPEED_INCREMENT = 50.f;


int main(int argc, char** argv)
{
    Window w(WINDOW_WIDTH, WINDOW_HEIGHT, "PONG");

    glm::vec2 ScreenCenter = w.GetScreenCenter();
    glm::vec3 PlayerOnePos(ScreenCenter.x - 350.f, ScreenCenter.y, 0.f);
    glm::vec3 PlayerTwoPos(ScreenCenter.x + 350.f, ScreenCenter.y, 0.f);
    glm::vec3 PlayerSize(6.5f, 50.f, 1.f);

    Transform PlayerOne(PlayerOnePos, PlayerSize);
    Transform PlayerTwo(PlayerTwoPos, PlayerSize);

    glm::vec3 BallBasePos(ScreenCenter.x, ScreenCenter.y, 1.f);
    glm::vec3 BallSize(10.f, 12.f, 1.f);
    glm::vec3 BallDirection(1.f, 0.5f, 0.f);

    Transform BallTransform(BallBasePos, BallSize);

    Game g(&w, PlayerOne, PlayerTwo, PLAYER_SPEED, BallTransform, BallDirection, BALL_BASE_SPEED, BALL_SPEED_INCREMENT, BALL_MAX_SPEED);

    try
    {
        w.Initialize();
        w.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        g.Begin();
    } catch (const Window::Error& Error)
    {
        std::cout << Error.what() << "\n";
        return -1;
    } catch (const Shader::ShaderCompileError& ShaderError)
    {
        std::cout << "ERROR: UNABLE TO COMPILE SHADER: " << ShaderError.what() << "\n";
        return -1;
    } catch (const Font::LoadError& FontError)
    {
        std::cout << FontError.what() << "\n";
        return -1;
    }

    // Render loop
    while (!g.ShouldClose())
    {
        g.Frame();
    }

    return 0;
}
