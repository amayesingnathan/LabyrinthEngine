#include "Lpch.h"
#include "Labyrinth/Tools/PlatformUtils.h"

#include "Labyrinth/Core/Application.h"

#ifdef LAB_PLATFORM_LINUX

#include <GLFW/glfw3.h>

namespace Labyrinth {

	//Linux File Dialog Definition
	std::string FileDialogs::OpenFile(const char* filter)
	{
		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		return std::string();
	}

	float Stopwatch::GetTime()
	{
		return (float)glfwGetTime();
	}

}
#endif