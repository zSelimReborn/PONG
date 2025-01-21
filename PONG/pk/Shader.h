#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
	typedef std::shared_ptr<Shader> SharedPtr;

	Shader();
	~Shader();

	bool IsCompiled() const;
	unsigned int GetShaderId() const;

	void Compile(const std::string& vertexShader, const std::string& fragmentShader);
	void Use() const;

	void SetBool(const std::string& name, const bool value) const;
	void SetInt(const std::string& name, const int value) const;
	void SetFloat(const std::string& name, const float value) const;
	void SetFloat(const std::string& name, const float values[]) const;
	void SetFloat(const std::string& name, const glm::vec2& value) const;
	void SetFloat(const std::string& name, const glm::vec3& value) const;
	void SetFloat(const std::string& name, const glm::vec4& value) const;
	void SetColor(const std::string& name, const float values[]) const;
	void SetMatrix(const std::string& name, const glm::mat4& matrix) const;

	class ShaderCompileError : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

private:
	void Initialize(const std::string&, const std::string&);

	std::string GetShaderContent(const std::string&) const;
	unsigned int CompileShader(const GLenum type, const std::string& content);

	int GetUniformLocation(const std::string& name) const;

	unsigned int shaderId;
	bool bIsCompiled;
};

