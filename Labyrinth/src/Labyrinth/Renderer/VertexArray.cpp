#include "Lpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLVertexArray.h"

namespace Labyrinth {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:		LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::OpenGL:	return new OpenGLVertexArray();
		}

		LAB_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

}