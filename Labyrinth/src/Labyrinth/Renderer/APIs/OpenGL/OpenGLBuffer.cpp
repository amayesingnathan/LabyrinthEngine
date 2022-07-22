#include "Lpch.h"
#include "OpenGLBuffer.h"

#include "glad/glad.h"

namespace Labyrinth {

	///
	/// OpenGL Vertex Buffer
	/// 

	OpenGLVertexBuffer::OpenGLVertexBuffer(u32 size)
	{
		LAB_PROFILE_FUNCTION();

		glGenBuffers(1, &mRendererID);
		glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(f32* vertices, u32 size)
	{
		LAB_PROFILE_FUNCTION();

		glGenBuffers(1, &mRendererID);
		glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		LAB_PROFILE_FUNCTION();

		glDeleteBuffers(1, &mRendererID);
	}

	void OpenGLVertexBuffer::bind() const
	{
		LAB_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
	}

	void OpenGLVertexBuffer::unbind() const
	{
		LAB_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::setData(const void* data, u32 size)
	{
		LAB_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}




	///
	/// OpenGL Index Buffer
	/// 

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32* indices, u32 count)
		: mCount(count)
	{
		LAB_PROFILE_FUNCTION();

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glGenBuffers(1, &mRendererID);
		glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		LAB_PROFILE_FUNCTION();

		glDeleteBuffers(1, &mRendererID);
	}

	void OpenGLIndexBuffer::bind() const
	{
		LAB_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID);
	}

	void OpenGLIndexBuffer::unbind() const
	{
		LAB_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}