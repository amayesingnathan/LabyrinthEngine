#pragma once

#include <cstdint>

namespace Labyrinth {

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setData(const void* data, uint32_t count) = 0;

		virtual uint32_t getCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};
}