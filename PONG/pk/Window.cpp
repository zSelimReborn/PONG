#include "Window.h"

#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

Window::Window(const int _Width, const int _Height, const std::string& _Title)
	: Width(_Width), Height(_Height), Title(_Title), WindowPtr(nullptr)
{
}

void Window::Initialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL major version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL minor version 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL Core profile

    GLFWwindow* window = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        throw Error("Failed to create GLFW window");
    }

    WindowPtr = window;
    glfwMakeContextCurrent(WindowPtr);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw Error("Failed to initialize GLAD");
    }

    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

    glEnable(GL_BLEND);
    SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int Window::GetWidth() const
{
	return Width;
}

int Window::GetHeight() const
{
	return Height;
}

glm::vec2 Window::GetScreenCenter() const
{
    return {Width / 2, Height / 2};
}

std::string Window::GetTitle() const
{
	return Title;
}

GLFWwindow* Window::GetWindow() const
{
    return WindowPtr;
}

void Window::Maximize() const
{
    glfwMaximizeWindow(WindowPtr);
}

void Window::ShouldClose(int Value) const
{
    glfwSetWindowShouldClose(WindowPtr, Value);
}

int Window::ShouldClose() const
{
    return glfwWindowShouldClose(WindowPtr);
}

void Window::CloseFrame() const
{
    glfwSwapBuffers(WindowPtr);
    glfwPollEvents();
}

bool Window::IsPressed(int Key) const
{
    return glfwGetKey(WindowPtr, Key) == GLFW_PRESS;
}

bool Window::IsReleased(int Key) const
{
    return glfwGetKey(WindowPtr, Key) == GLFW_RELEASE;
}

void Window::ClearColor(const float Color[]) const
{
    glClearColor(Color[0], Color[1], Color[2], Color[3]);
}

void Window::ClearFlags(int Flags) const
{
    glClear(Flags);
}

void Window::SetBlendFunction(int Key, int Value) const
{
    glBlendFunc(Key, Value);
}

void Window::SetInputMode(const int Mode, const int Value) const
{
	glfwSetInputMode(WindowPtr, Mode, Value);
}

Window::~Window()
{
    glfwTerminate();
}

void Window::FrameBufferSizeCallback(GLFWwindow* Window, int _Width, int _Height)
{
    glad_glViewport(0, 0, _Width, _Height);
}
