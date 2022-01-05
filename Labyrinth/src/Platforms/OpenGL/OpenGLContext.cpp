#include "Lpch.h"
#include "OpenGLContext.h"

namespace Labyrinth {

	OpenGLContext::OpenGLContext(SDL_Window* windowHandle)
		: mWindowHandle(windowHandle)
	{
		LAB_CORE_ASSERT(windowHandle, "Window handle is null!");

		mContextHandle = SDL_GL_CreateContext(mWindowHandle);
		LAB_CORE_ASSERT(mContextHandle, "Could not create OpenGL context!");
	}

	void OpenGLContext::init()
	{
		SDL_GL_MakeCurrent(mWindowHandle, mContextHandle);
		int status = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		LAB_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::swapBuffers()
	{
		SDL_GL_SwapWindow(mWindowHandle);
	}
}