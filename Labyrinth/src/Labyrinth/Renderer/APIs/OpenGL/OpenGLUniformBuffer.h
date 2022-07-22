#pragma once

#include "Labyrinth/Renderer/UniformBuffer.h"

namespace Labyrinth {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(u32 size, u32 binding);
		virtual ~OpenGLUniformBuffer();

		virtual void setData(const void* data, u32 size, u32 offset = 0) override;

	private:
		u32 m_RendererID = 0;
	};
}