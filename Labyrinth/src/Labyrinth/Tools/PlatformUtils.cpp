#include "Lpch.h"
#include "Labyrinth/Tools/PlatformUtils.h"

#include "Labyrinth/Core/Application.h"

#include <portable-file-dialogs.h>
#include <GLFW/glfw3.h>

namespace Labyrinth {

	std::string FileDialogs::OpenFile(const std::vector<std::string>& filter)
	{
        auto selection = pfd::open_file("Select a file", ".", filter).result();
        if (!selection.empty())
            return selection[0];

        return std::string();
	}

	std::string FileDialogs::SaveFile(const std::vector<std::string>& filter)
	{
		auto destination = pfd::save_file("Save file as", ".", filter).result();
        return destination;
	}

	float Stopwatch::GetTime()
	{
		return (float)glfwGetTime();
	}

}