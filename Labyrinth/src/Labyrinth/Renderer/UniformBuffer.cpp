#include "Lpch.h"
#include "UniformBuffer.h"

#include <glad/glad.h>

namespace Laby {

	UniformBuffer::UniformBuffer(u32 size, u32 binding)
	{
		glCreateBuffers(1, &mRendererID);
		glNamedBufferData(mRendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererID);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &mRendererID);
	}

	void UniformBuffer::SetData(const void* data, u32 size, u32 offset)
	{
		glNamedBufferSubData(mRendererID, offset, size, data);
	}
}

