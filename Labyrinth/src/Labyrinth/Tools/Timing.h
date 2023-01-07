#pragma once

#include <chrono>

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	class Stopwatch
	{
	public:
		static f32 GetTime();
	};

	class Timer
	{
	public:
		Timer()
		{
			reset();
		}

		void reset()
		{
			mStart = std::chrono::steady_clock::now();
		}

		f32 elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - mStart).count() * 0.001f * 0.001f * 0.001f;
		}

		f32 elapsedMillis()
		{
			return elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> mStart;
	};

}