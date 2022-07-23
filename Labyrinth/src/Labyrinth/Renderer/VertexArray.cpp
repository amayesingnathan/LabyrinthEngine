#include "Lpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "OpenGL/OpenGLVertexArray.h"

namespace Labyrinth {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return Ref<OpenGLVertexArray>::Create();
		}

		LAB_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

}