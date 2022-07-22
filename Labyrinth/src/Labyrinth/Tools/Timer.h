#pragma once

#include <chrono>

namespace Labyrinth {

	class LAB_API Timer
	{
	public:
		Timer()
		{
			reset();
		}

		void reset()
		{
			mStart = std::chrono::high_resolution_clock::now();
		}

		f32 elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * 0.001f * 0.001f * 0.001f;
		}

		f32 elapsedMillis()
		{
			return elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	};

}