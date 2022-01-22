#include "Lpch.h"
#include "OpenGLContext.h"

namespace Labyrinth {

	OpenGLContext::OpenGLContext(SDL_Window* windowHandle)
		: mWindowHandle(windowHandle)
	{
		LAB_PROFILE_FUNCTION();

		LAB_CORE_ASSERT(windowHandle, "Window handle is null!");

		mContextHandle = SDL_GL_CreateContext(mWindowHandle);
		LAB_CORE_ASSERT(mContextHandle, "Could not create OpenGL context!");
	}

	void OpenGLContext::init()
	{
		LAB_PROFILE_FUNCTION();

		SDL_GL_MakeCurrent(mWindowHandle, mContextHandle);
		int status = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		LAB_CORE_ASSERT(status, "Failed to initialize Glad!");

		LAB_CORE_INFO("OpenGL Info:");
		LAB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		LAB_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		LAB_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

#ifdef LAB_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		LAB_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Labyrinth requires at least OpenGL version 4.5!");
#endif
	}

	void OpenGLContext::swapBuffers()
	{
		LAB_PROFILE_FUNCTION();

		SDL_GL_SwapWindow(mWindowHandle);
	}
}