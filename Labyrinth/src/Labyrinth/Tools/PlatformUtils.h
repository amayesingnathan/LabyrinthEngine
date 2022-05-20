#pragma once

#include <string>

namespace Labyrinth {

	class LAB_API FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

	class LAB_API Stopwatch
	{
	public:
		static float GetTime();
	};
}