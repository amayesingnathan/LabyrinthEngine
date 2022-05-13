#include "Lpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Labyrinth {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: mWindowHandle(windowHandle)
	{
		LAB_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::init()
	{
		LAB_PROFILE_FUNCTION();

		glfwMakeContextCurrent(mWindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LAB_CORE_ASSERT(status, "Failed to initialize Glad!");

		LAB_CORE_INFO("OpenGL Info:");
		LAB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		LAB_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		LAB_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		LAB_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Labyrinth requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::swapBuffers()
	{
		LAB_PROFILE_FUNCTION();

		glfwSwapBuffers(mWindowHandle);
	}
}