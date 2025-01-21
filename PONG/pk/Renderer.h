#pragma once

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

class Renderer
{
public:
	static Renderer& Get()
	{
		static Renderer Instance;
		return Instance;
	}

	void RenderSprite(const Shader::SharedPtr& Shader, const Texture::SharedPtr& Texture, const glm::mat4& Model, const glm::vec3& Color) const;

private:
	Renderer();
	void InitializeSpriteQuad();

	unsigned int SpriteQuadId;
};