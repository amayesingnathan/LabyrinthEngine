#include "Lpch.h"
#include "Utils.h"

#include <GLFW/glfw3.h>

namespace Laby {

	f32 Stopwatch::GetTime()
	{
		return (f32)glfwGetTime();
	}

}