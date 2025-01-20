#include "SoundEngine.h"

#include <iostream>

QueuedSound::QueuedSound()
	: SoundObject(nullptr), Channel(nullptr)
{
}

QueuedSound::QueuedSound(FMOD::Sound* _SoundObject, FMOD::Channel* _Channel)
	: SoundObject(_SoundObject), Channel(_Channel)
{
}

void SoundEngine::Load(const std::string& SoundPath)
{
	if (!System)
	{
		return;
	}

	if (LoadedSounds.count(SoundPath) > 0)
	{
		return;
	}

	FMOD_MODE Mode = FMOD_DEFAULT;
	Mode |= FMOD_2D;
	Mode |= FMOD_LOOP_OFF;
	Mode |= FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* SoundObject = nullptr;
	System->createSound(SoundPath.c_str(), Mode, nullptr, &SoundObject);
	LoadedSounds.insert(std::pair<std::string, FMOD::Sound*>(SoundPath, SoundObject));
}

bool SoundEngine::Play(const std::string& SoundPath)
{
	if (!System)
	{
		return false;
	}

	if (LoadedSounds.count(SoundPath) <= 0)
	{
		Load(SoundPath);
	}

	FMOD::Sound* SoundObject = LoadedSounds[SoundPath];
	FMOD::Channel* Channel = nullptr;
	LastResult = System->playSound(SoundObject, nullptr, false, &Channel);
	if (LastResult == FMOD_OK)
	{
		Queue.emplace_back(SoundObject, Channel);
		return true;
	}

	return false;
}

FMOD_RESULT SoundEngine::GetLastResult() const
{
	return LastResult;
}

void SoundEngine::Update(const float Delta)
{
	std::vector<QueueIterator> StoppedSounds;

	for (QueueIterator It = Queue.begin(); It != Queue.end(); ++It)
	{
		bool bIsPlaying = false;
		It->Channel->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			StoppedSounds.push_back(It);
		}
	}

	for (const QueueIterator& It : StoppedSounds)
	{
		Queue.erase(It);
	}

	System->update();
}

SoundEngine::~SoundEngine()
{
	System->release();
}

SoundEngine::SoundEngine()
{
	Initialize();
}

void SoundEngine::Initialize()
{
	LastResult = FMOD::System_Create(&System);

	System->init(32, FMOD_INIT_NORMAL, nullptr);
}
