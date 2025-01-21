#pragma once

#include <fmod/fmod.hpp>

#include <map>
#include <string>

class SoundEngine
{
public:
	typedef std::map<std::string, FMOD::Sound*> SoundsMap;
	typedef std::pair<std::string, FMOD::Sound*> SoundPair;
	typedef std::map<int, FMOD::Channel*> ChannelsMap;
	typedef std::pair<int, FMOD::Channel*> ChannelPair;

	static SoundEngine& Get()
	{
		static SoundEngine Instance;
		return Instance;
	}

	void Load(const std::string& SoundPath);
	int Play(const std::string& SoundPath, const float Volume);
	void SetChannelVolume(const int ChannelId, const float NewVolume);
	void SetChannelPitch(const int ChannelId, const float Pitch);

	FMOD_RESULT GetLastResult() const;

	void Update(const float Delta);

	SoundEngine(const SoundEngine&) = delete;
	void operator=(const SoundEngine&) = delete;

	~SoundEngine();

private:
	SoundEngine();
	void Initialize();

	FMOD::System* System;
	FMOD_RESULT LastResult;

	SoundsMap LoadedSounds;
	ChannelsMap ActiveChannels;
	int NextChannelId;
};