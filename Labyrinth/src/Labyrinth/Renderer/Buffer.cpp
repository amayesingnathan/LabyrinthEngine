#include "Lpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "OpenGL/OpenGLBuffer.h"

namespace Labyrinth {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(size);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

	Ref<VertexBuffer> Labyrinth::VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

	Ref<IndexBuffer> Labyrinth::IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

}
