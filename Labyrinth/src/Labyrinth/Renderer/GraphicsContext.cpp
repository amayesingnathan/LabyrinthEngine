#include "Lpch.h"
#include "GraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Laby {

	GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
		: mWindowHandle(windowHandle)
	{
		LAB_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void GraphicsContext::init()
	{
		glfwMakeContextCurrent(mWindowHandle);
		i32 status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LAB_CORE_ASSERT(status, "Failed to initialize Glad!");

		const char* vendor = (char*)glGetString(GL_VENDOR);
		const char* renderer = (char*)glGetString(GL_RENDERER);
		const char* version = (char*)glGetString(GL_VERSION);

		LAB_CORE_INFO("OpenGL Info:");
		LAB_CORE_INFO("  Vendor: {0}", vendor);
		LAB_CORE_INFO("  Renderer: {0}", renderer);
		LAB_CORE_INFO("  Version: {0}", version);

		LAB_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Labyrinth requires at least OpenGL version 4.5!");
	}

	void GraphicsContext::swapBuffers()
	{
		glfwSwapBuffers(mWindowHandle);
	}

}