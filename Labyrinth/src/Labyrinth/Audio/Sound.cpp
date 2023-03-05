#include "Lpch.h"
#include "Sound.h"

#include "AudioEngine.h"

namespace Laby {

	Sound::Sound(StreamType type, const fs::path& filename)
		: mSound(AudioEngine::CreateSound(type, filename).mSound)
	{
	}
}
