#include "SoundEngine.h"

#include <iostream>
#include <vector>

#include "Common.h"

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

int SoundEngine::Play(const std::string& SoundPath, const float Volume)
{
	if (!System)
	{
		return false;
	}

	if (LoadedSounds.count(SoundPath) <= 0)
	{
		Load(SoundPath);
	}

	const float ActualVolume = Math::Clamp(Volume, 0.f, 1.f);
	FMOD::Sound* SoundObject = LoadedSounds[SoundPath];
	FMOD::Channel* Channel = nullptr;
	LastResult = System->playSound(SoundObject, nullptr, false, &Channel);
	if (LastResult == FMOD_OK)
	{
		Channel->setVolume(ActualVolume);
		ActiveChannels.insert(ChannelPair(NextChannelId++, Channel));
		return true;
	}

	return false;
}

void SoundEngine::SetChannelVolume(const int ChannelId, const float NewVolume)
{
	if (ActiveChannels.count(ChannelId) <= 0)
	{
		return;
	}

	const float ActualVolume = Math::Clamp(NewVolume, 0.f, 1.f);
	ActiveChannels[ChannelId]->setVolume(ActualVolume);
}

void SoundEngine::SetChannelPitch(const int ChannelId, const float Pitch)
{
	if (ActiveChannels.count(ChannelId) <= 0)
	{
		return;
	}

	ActiveChannels[ChannelId]->setPitch(Pitch);
}

FMOD_RESULT SoundEngine::GetLastResult() const
{
	return LastResult;
}

void SoundEngine::Update(const float Delta)
{
	std::vector<ChannelsMap::iterator> StoppedSounds;

	for (ChannelsMap::iterator It = ActiveChannels.begin(), End = ActiveChannels.end(); It != End; ++It)
	{
		bool bIsPlaying = false;
		It->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			StoppedSounds.push_back(It);
		}
	}

	for (ChannelsMap::iterator& It : StoppedSounds)
	{
		ActiveChannels.erase(It);
	}

	System->update();
}

SoundEngine::~SoundEngine()
{
	System->release();
}

SoundEngine::SoundEngine()
	: NextChannelId(0)
{
	Initialize();
}

void SoundEngine::Initialize()
{
	LastResult = FMOD::System_Create(&System);

	System->init(32, FMOD_INIT_NORMAL, nullptr);
}
