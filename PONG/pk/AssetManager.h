#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Font.h"

class AssetManager
{
public:
	typedef std::map<std::string, Shader::SharedPtr> ShaderMap;
	typedef std::pair<std::string, Shader::SharedPtr> ShaderPair;

	typedef std::map<std::string, Texture::SharedPtr> TextureMap;
	typedef std::pair<std::string, Texture::SharedPtr> TexturePair;

	typedef std::map<std::string, Font::SharedPtr> FontMap;
	typedef std::pair<std::string, Font::SharedPtr> FontPair;

	static AssetManager& Get()
	{
		static AssetManager Instance;
		return Instance;
	}

	Shader::SharedPtr LoadShader(const std::string& Name, const std::string& Vertex, const std::string& Fragment);
	Texture::SharedPtr LoadTexture(const std::string& Name, const std::string& Path, int _Format, int _WrapS, int _WrapT, int _MinFilter, int _MaxFilter);
	Font::SharedPtr LoadFont(const std::string& Name, const std::string& Path, const std::string& ShaderName, const glm::mat4& _Projection);

	Shader::SharedPtr GetShader(const std::string& Name);
	Texture::SharedPtr GetTexture(const std::string& Name);
	Font::SharedPtr GetFont(const std::string& Name);

private:
	ShaderMap Shaders;
	TextureMap Textures;
	FontMap Fonts;
};