#pragma once

#include <memory>
#include <stdexcept>
#include <string>

class Texture
{
public:
	typedef std::shared_ptr<Texture> SharedPtr;

	Texture(const std::string& _Path, int _Format, int _WrapS, int _WrapT, int _MinFilter, int _MaxFilter);

	unsigned int GetId() const;
	std::string GetPath() const;
	int GetWidth() const;
	int GetHeight() const;

	void Bind() const;
	void UnBind() const;

	class LoadError : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

private:
	unsigned int Id;
	std::string Path;

	int Width;
	int Height;
	int Channels;
	int Format;
	int WrapS;
	int WrapT;
	int MinFilter;
	int MaxFilter;
};