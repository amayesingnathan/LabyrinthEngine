#include "Lpch.h"
#include "AudioEngine.h"

#include <fmod.hpp>
#include <fmod_errors.h>

namespace Laby {

	static bool FMOD_CheckError(FMOD_RESULT result)
	{
		if (result == FMOD_OK)
			return false;

		LAB_CORE_ERROR("FMOD error [{}] - {}", result, FMOD_ErrorString(result));
		return true;
	}

	bool AudioEngine::Init()
	{
		if (sEngineState)
			LAB_CORE_WARN("Audio Engine already intialised!");

		sEngineState = MakeSingle<AudioEngineState>();

		if (FMOD_CheckError(FMOD::System_Create(&sEngineState->system)))
			return false;

		if (FMOD_CheckError(sEngineState->system->init(512, FMOD_INIT_NORMAL, nullptr)))
			return false;

		return true;
	}

	Sound AudioEngine::CreateSound(StreamType type, fs::path filename)
	{
		if (!IsStateValid())
			return nullptr;

		FMOD::Sound* sound = nullptr;
		auto test = fs::current_path();

		FMOD_RESULT result = FMOD_OK;
		switch (type)
		{
		case StreamType::File:
			result = sEngineState->system->createStream(filename.string().c_str(), FMOD_DEFAULT, nullptr, &sound);
			break;
		case StreamType::Memory:
			result = sEngineState->system->createSound(filename.string().c_str(), FMOD_DEFAULT, nullptr, &sound);
			break;
		}
		FMOD_CheckError(result);
		return sound;
	}

	void AudioEngine::PlaySound(Sound sound)
	{
		if (!IsStateValid())
			return;

		sEngineState->system->playSound(sound.mSound, nullptr, false, nullptr);
	}

	bool AudioEngine::IsStateValid()
	{
		return sEngineState && sEngineState->system;
	}
}