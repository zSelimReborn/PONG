#pragma once

#include <fmod/fmod.hpp>

#include <map>
#include <vector>
#include <string>

struct QueuedSound
{
	FMOD::Sound* SoundObject;
	FMOD::Channel* Channel;

	QueuedSound();
	QueuedSound(FMOD::Sound* _SoundObject, FMOD::Channel* _Channel);
};

class SoundEngine
{
public:
	typedef std::vector<QueuedSound>::iterator QueueIterator;

	static SoundEngine& Get()
	{
		static SoundEngine Instance;
		return Instance;
	}

	void Load(const std::string& SoundPath);
	bool Play(const std::string& SoundPath);
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

	std::map<std::string, FMOD::Sound*> LoadedSounds;
	std::vector<QueuedSound> Queue;
};