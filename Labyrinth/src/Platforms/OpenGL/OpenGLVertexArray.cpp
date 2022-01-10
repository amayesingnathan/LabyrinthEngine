#include "Lpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Labyrinth {

	static GLenum ShaderDataTypetoOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
		}

		LAB_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		LAB_PROFILE_FUNCTION();

		glGenVertexArrays(1, &mRendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		LAB_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &mRendererID);
	}

	void OpenGLVertexArray::bind() const
	{
		LAB_PROFILE_FUNCTION();

		glBindVertexArray(mRendererID);
	}

	void OpenGLVertexArray::unbind() const
	{
		LAB_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		LAB_PROFILE_FUNCTION();

		LAB_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has no layout!");

		glBindVertexArray(mRendererID);
		vertexBuffer->bind();

		const auto& layout = vertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			switch (element.type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribPointer(mVertexBufferIndex,
					element.getComponentCount(),
					ShaderDataTypetoOpenGLType(element.type),
					element.normalised ? GL_TRUE : GL_FALSE,
					layout.getStride(),
					(const void*)element.offset);
				mVertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.getComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(mVertexBufferIndex);
					glVertexAttribPointer(mVertexBufferIndex,
						count,
						ShaderDataTypetoOpenGLType(element.type),
						element.normalised ? GL_TRUE : GL_FALSE,
						layout.getStride(),
						(const void*)(sizeof(float) * count * i));
					glVertexAttribDivisor(mVertexBufferIndex, 1);
					mVertexBufferIndex++;
				}
				break;
			}
			default:
				LAB_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
			/*glEnableVertexAttribArray(mVertexBufferIndex);
			glVertexAttribPointer(mVertexBufferIndex,
				element.getComponentCount(),
				ShaderDataTypetoOpenGLType(element.type),
				element.normalised ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)element.offset);
			mVertexBufferIndex++;*/
		}

		mVertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		LAB_PROFILE_FUNCTION();

		glBindVertexArray(mRendererID);
		indexBuffer->bind();

		mIndexBuffer = indexBuffer;
	}

}