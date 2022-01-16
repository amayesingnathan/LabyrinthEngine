#pragma once

#include "Labyrinth/Renderer/GraphicsContext.h"

#include <SDL.h>
#include <glad/glad.h>
#include "SDL_opengl.h"

namespace Labyrinth {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(SDL_Window* windowHandle);

		virtual void init() override;
		virtual void swapBuffers() override;

	private:
		SDL_Window* mWindowHandle;
		SDL_GLContext mContextHandle;
	};

}