#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

Texture::Texture(const std::string& _Path, int _Format, int _WrapS, int _WrapT, int _MinFilter, int _MaxFilter)
	: Path(_Path), Width(0), Height(0), Channels(0), Format(_Format), WrapS(_WrapS), WrapT(_WrapT), MinFilter(_MinFilter), MaxFilter(_MaxFilter)
{
	glGenTextures(1, &Id);
	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MaxFilter);

	unsigned char* Data = stbi_load(Path.c_str(), &Width, &Height, &Channels, 0);
	if (Data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
	{
		std::string ErrorMsg("Unable to load texture ");
		ErrorMsg += Path;
		throw LoadError(ErrorMsg);
	}

	stbi_image_free(Data);
}

unsigned int Texture::GetId() const
{
	return Id;
}

std::string Texture::GetPath() const
{
	return Path;
}

int Texture::GetWidth() const
{
	return Width;
}

int Texture::GetHeight() const
{
	return Height;
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, Id);
}

void Texture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
