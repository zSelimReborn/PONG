#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

void FrameBufferSizeCallback(GLFWwindow* window, int w, int h);
void ProcessInput(GLFWwindow* window, const float Delta);

unsigned int PrepareRectangle();

Shader MainShader;

constexpr float MiddleScreenX = WINDOW_WIDTH / 2;
constexpr float MiddleScreenY = WINDOW_HEIGHT / 2;

constexpr float PlayerSpeed = 150.f;
glm::vec3 PlayerOnePos(MiddleScreenX - 350.f, WINDOW_HEIGHT / 2, 0.f);
glm::vec3 PlayerTwoPos(MiddleScreenX + 350.f, WINDOW_HEIGHT / 2, 0.f);
glm::vec3 PlayerSize(10.f, 50.f, 1.f);

glm::vec3 BrickBasePos(MiddleScreenX, 0.f, 0.f);
glm::vec3 BrickSize(5.f, 20.f, 1.f);
float BrickSpan = 30.f;
int BrickQty = static_cast<int>(WINDOW_HEIGHT / (BrickSize.y));

int main(int argc, char** argv)
{
    glfwInit(); // Initialize GLFW to default
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL major version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL minor version 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL Core profile

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PONG", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << "\n";
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

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
    float White[] = { 1.f, 1.f, 1.f };

    MainShader.Use();
    MainShader.SetMatrix("projection", Ortho);
    MainShader.SetColor("spriteColor", White);

    glm::mat4 Identity(1.0f);

    unsigned int Quad = PrepareRectangle();

    double OldTime = glfwGetTime();
    double CurrentTime = OldTime;
    double Delta = CurrentTime - OldTime;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        CurrentTime = glfwGetTime();
        Delta = CurrentTime - OldTime;
        OldTime = CurrentTime;

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ProcessInput(window, Delta);

        glm::mat4 PlayerOneModel = glm::translate(Identity, PlayerOnePos);
        glm::mat4 PlayerTwoModel = glm::translate(Identity, PlayerTwoPos);
        PlayerOneModel = glm::scale(PlayerOneModel, PlayerSize);
        PlayerTwoModel = glm::scale(PlayerTwoModel, PlayerSize);

        glBindVertexArray(Quad);

        MainShader.SetMatrix("model", PlayerOneModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        MainShader.SetMatrix("model", PlayerTwoModel);
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void FrameBufferSizeCallback(GLFWwindow* window, int w, int h)
{
    glad_glViewport(0, 0, w, h);
}

void ProcessInput(GLFWwindow* window, const float Delta)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        PlayerOnePos.y -= PlayerSpeed * Delta;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        PlayerOnePos.y += PlayerSpeed * Delta;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        PlayerTwoPos.y -= PlayerSpeed * Delta;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        PlayerTwoPos.y += PlayerSpeed * Delta;
    }
}

unsigned int PrepareRectangle()
{
    float rectangle[] = {
        -0.5f, 0.7f,
        -0.5f, -0.7f,
        0.5f, -0.7f,
		-0.5f, 0.7f,
        0.5f, 0.7f,
        0.5f, -0.7f
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
