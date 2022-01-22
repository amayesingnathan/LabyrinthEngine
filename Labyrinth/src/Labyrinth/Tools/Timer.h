@@ - 0, 0 + 1, 34 @@
#pragma once

#include <chrono>

namespace Labyrinth {

	class Timer
	{
	public:
		Timer()
		{
			reset();
		}

		void Timer::reset()
		{
			mStart = std::chrono::high_resolution_clock::now();
		}

		float Timer::elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * 0.001f * 0.001f * 0.001f;
		}

		float Timer::elapsedMillis()
		{
			return elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	};

}