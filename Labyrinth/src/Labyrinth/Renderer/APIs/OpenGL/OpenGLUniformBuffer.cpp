#include "Lpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Labyrinth {

	OpenGLUniformBuffer::OpenGLUniformBuffer(u32 size, u32 binding)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::setData(const void* data, u32 size, u32 offset)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

}