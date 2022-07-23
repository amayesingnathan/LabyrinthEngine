#include "Lpch.h"
#include "UniformBuffer.h"

#include "Labyrinth/Renderer/Renderer.h"
#include "OpenGL/OpenGLUniformBuffer.h"

namespace Labyrinth {

	Ref<UniformBuffer> UniformBuffer::Create(u32 size, u32 binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Ref<OpenGLUniformBuffer>::Create(size, binding);
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}