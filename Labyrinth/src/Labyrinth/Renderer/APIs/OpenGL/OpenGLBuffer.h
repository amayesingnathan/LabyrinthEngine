#pragma once

#include "Labyrinth/Renderer/Buffer.h"

namespace Labyrinth {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(u32 size);
		OpenGLVertexBuffer(f32* vertices, u32 size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setData(const void* data, u32 size) override;

		virtual const BufferLayout& getLayout() const override { return mLayout; }
		virtual void setLayout(const BufferLayout& layout) override { mLayout = layout; }

	private:
		u32 mRendererID;
		BufferLayout mLayout;
	};

	//Labyrinth only supports 32-bit index buffers
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(u32* indices, u32 count);
		virtual ~OpenGLIndexBuffer() override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual u32 getCount() const override { return mCount; }

	private:
		u32 mRendererID;
		u32 mCount;
	};
}