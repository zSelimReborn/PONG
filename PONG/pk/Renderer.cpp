#include "Renderer.h"

#include <glad/glad.h>

Renderer::Renderer()
{
	InitializeSpriteQuad();
}

void Renderer::InitializeSpriteQuad()
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

	SpriteQuadId = VAO;
}

void Renderer::RenderSprite(const Shader::SharedPtr& Shader, const Texture::SharedPtr& Texture, const glm::mat4& Model, const glm::vec3& Color) const
{
	glBindVertexArray(SpriteQuadId);

	if (Shader)
	{
		Shader->Use();
		Shader->SetFloat("spriteColor", Color);
		Shader->SetMatrix("model", Model);
	}

	if (Texture)
	{
		glActiveTexture(GL_TEXTURE0);
		Texture->Bind();
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	if (Texture)
	{
		Texture->UnBind();
	}
}

