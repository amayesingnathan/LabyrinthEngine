#pragma once

#include <Labyrinth/Core/System/Base.h>

#include "AudioEngineInternal.h"
#include "Sound.h"

namespace Laby {

	class AudioEngine
	{
	public:
		static bool Init();

		static Sound CreateSound(StreamType type, fs::path filename);
		static void PlaySound(Sound sound);

	private:
		static bool IsStateValid();

	private:
		inline static Single<AudioEngineState> sEngineState = nullptr;
	};
}