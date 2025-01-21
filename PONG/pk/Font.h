#pragma once

#include <string>
#include <stdexcept>
#include <map>
#include <memory>

#include "Shader.h"

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

class Font
{
public:
	typedef std::shared_ptr<Font> SharedPtr;

	Font(const std::string& _Path, const std::string& _Name, const glm::mat4& _Projection, const Shader::SharedPtr& _TextShader);

	std::string GetName() const;
	std::string GetPath() const;
	unsigned int GetSize() const;

	void Load(unsigned int _Size);
	void Render(const std::string& Text, const glm::vec2& Position, float Scale, const float Color[]);

	class LoadError : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

private:
	void LoadCharacters(FT_Face& Face);
	void PrepareRenderQuad();

	std::string Path;
	std::string Name;
	unsigned int Size;

	std::map<char, Character> Characters;

	Shader::SharedPtr TextShader;
	unsigned int QuadId;
	unsigned int BufferId;

	glm::mat4 Projection;

	bool bLoaded = false;
};