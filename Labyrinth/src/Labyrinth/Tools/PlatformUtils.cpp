#include "Lpch.h"
#include "Labyrinth/Tools/PlatformUtils.h"

#include <GLFW/glfw3.h>

namespace Labyrinth {

	f32 Stopwatch::GetTime()
	{
		return (f32)glfwGetTime();
	}

}