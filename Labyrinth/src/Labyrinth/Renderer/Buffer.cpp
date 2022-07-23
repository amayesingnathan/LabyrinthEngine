#include "Lpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "OpenGL/OpenGLBuffer.h"

namespace Labyrinth {

	Ref<VertexBuffer> VertexBuffer::Create(u32 size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return Ref<OpenGLVertexBuffer>::Create(size);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

	Ref<VertexBuffer> Labyrinth::VertexBuffer::Create(f32* vertices, u32 size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return Ref<OpenGLVertexBuffer>::Create(vertices, size);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

	Ref<IndexBuffer> Labyrinth::IndexBuffer::Create(u32* indices, u32 count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return Ref<OpenGLIndexBuffer>::Create(indices, count);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

}
