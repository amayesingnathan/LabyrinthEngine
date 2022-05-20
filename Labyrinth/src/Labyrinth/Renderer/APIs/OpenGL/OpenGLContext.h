#pragma once

#include "Labyrinth/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Labyrinth {

	class LAB_API OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void init() override;
		virtual void swapBuffers() override;

	private:
		GLFWwindow* mWindowHandle;
	};

}