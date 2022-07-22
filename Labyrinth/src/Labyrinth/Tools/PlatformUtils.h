#pragma once

#include <string>

namespace Labyrinth {

	class LAB_API FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const std::vector<std::string>& filter);
		static std::string SaveFile(const std::vector<std::string>& filter);
	};

	class LAB_API Stopwatch
	{
	public:
		static f32 GetTime();
	};
}