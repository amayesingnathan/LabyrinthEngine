#pragma once

#include <Labyrinth/IO/Filesystem.h>

#include "AudioDefinitions.h"

namespace Laby {

	enum class StreamType
	{
		Memory, File
	};

	class Sound
	{
	public:
		Sound(StreamType type, const fs::path& filename);
		Sound(FMOD::Sound* sound) : mSound(sound) {}

	private:
		FMOD::Sound* mSound;

		friend class AudioEngine;
	};
}