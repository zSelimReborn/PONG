#include "Shader.h"

#include <sstream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : shaderId(0), bIsCompiled(false)
{
}

Shader::~Shader()
{
    glDeleteProgram(shaderId);
}

bool Shader::IsCompiled() const
{
    return bIsCompiled;
}

unsigned int Shader::GetShaderId() const
{
    return shaderId;
}

void Shader::Compile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    bIsCompiled = false;

    Initialize(vertexShaderPath, fragmentShaderPath);

    bIsCompiled = true;
}

void Shader::Use() const
{
    glUseProgram(shaderId);
}

void Shader::SetBool(const std::string& name, const bool value) const
{
    const int location = GetUniformLocation(name);
    glUniform1i(location, (int)value);
}

void Shader::SetInt(const std::string& name, const int value) const
{
    const int location = GetUniformLocation(name);
    glUniform1i(location, (int)value);
}

void Shader::SetFloat(const std::string& name, const float value) const
{
    const int location = GetUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::SetFloat(const std::string& name, const float values[]) const
{
    const int location = GetUniformLocation(name);
    glUniform4f(location, values[0], values[1], values[2], values[3]);
}

void Shader::SetFloat(const std::string& name, const glm::vec2& value) const
{
    const int location = GetUniformLocation(name);
    glUniform2f(location, value.x, value.y);
}

void Shader::SetFloat(const std::string& name, const glm::vec3& value) const
{
    const int location = GetUniformLocation(name);
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetFloat(const std::string& name, const glm::vec4& value) const
{
    const int location = GetUniformLocation(name);
    glUniform4f(location, value.x, value.y, value.z, value.a);
}

void Shader::SetColor(const std::string& name, const float values[]) const
{
    const int location = GetUniformLocation(name);
    glUniform3f(location, values[0], values[1], values[2]);
}

void Shader::SetMatrix(const std::string& name, const glm::mat4& matrix) const
{
    const int location = GetUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::Initialize(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    const std::string vertexShader = GetShaderContent(vertexShaderPath);
    const std::string fragmentShader = GetShaderContent(fragmentShaderPath);

    const unsigned int vertexShaderId = CompileShader(GL_VERTEX_SHADER, vertexShader);
    const unsigned int fragmentShaderId = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertexShaderId);
    glAttachShader(shaderId, fragmentShaderId);
    glLinkProgram(shaderId);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderId, 512, NULL, infoLog);
        std::string error = "[Shader] - Linking failed: ";
        error += infoLog;
        throw ShaderCompileError(error);
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
}

std::string Shader::GetShaderContent(const std::string& shaderFile) const
{
    std::ifstream handler;
    handler.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    try {
        handler.open(shaderFile);
        std::stringstream content;

        content << handler.rdbuf();
        handler.close();

        return content.str();
    }
    catch (const std::ifstream::failure& e) {
        std::string error = "[Shader] - Error reading content: ";
        error += e.what();
        throw ShaderCompileError(error);
    }
}

unsigned int Shader::CompileShader(const GLenum type, const std::string& content)
{
    unsigned int shader;
    shader = glCreateShader(type);

    const char* shaderContent = content.c_str();
    glShaderSource(shader, 1, &shaderContent, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string error = "[Shader] - Compilation failed: ";
        error += infoLog;
        throw ShaderCompileError(error);
    }

    return shader;
}

int Shader::GetUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(shaderId, name.c_str());
}
