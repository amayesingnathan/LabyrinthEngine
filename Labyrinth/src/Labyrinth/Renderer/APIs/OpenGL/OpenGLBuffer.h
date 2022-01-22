#pragma once

#include "Labyrinth/Renderer/Buffer.h"

namespace Labyrinth {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setData(const void* data, uint32_t size) override;

		virtual const BufferLayout& getLayout() const override { return mLayout; }
		virtual void setLayout(const BufferLayout& layout) override { mLayout = layout; }

	private:
		uint32_t mRendererID;
		BufferLayout mLayout;
	};

	//Labyrinth only supports 32-bit index buffers
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer() override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setData(const void* data, uint32_t count) override;

		virtual uint32_t getCount() const override { return mCount; }

	private:
		uint32_t mRendererID;
		uint32_t mCount;
	};
}