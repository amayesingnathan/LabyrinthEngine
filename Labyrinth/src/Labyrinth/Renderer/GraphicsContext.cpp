#include "Lpch.h"
#include "Labyrinth/Renderer/GraphicsContext.h"

#include "Labyrinth/Renderer/Renderer.h"
#include "OpenGL/OpenGLContext.h"

namespace Labyrinth {

	Single<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateSingle<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}