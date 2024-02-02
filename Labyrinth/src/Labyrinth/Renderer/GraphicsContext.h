#pragma once

#include <Labyrinth/Core/System/Ref.h>

struct GLFWwindow;

namespace Laby {

	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);

		virtual void Init();
		virtual void SwapBuffers();

		static Single<GraphicsContext> Create(void* window) { return MakeSingle<GraphicsContext>((GLFWwindow*)window); }

	private:
		GLFWwindow* mWindowHandle;
	};

}