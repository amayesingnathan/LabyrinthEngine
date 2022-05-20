#pragma once

#include "Labyrinth/Renderer/VertexArray.h"

namespace Labyrinth {

	class LAB_API OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return mVertexBuffers; }
		virtual const Ref<IndexBuffer>& getIndexBuffer() const { return mIndexBuffer; }

	private:
		uint32_t mRendererID;
		uint32_t mVertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> mVertexBuffers;
		Ref<IndexBuffer> mIndexBuffer;

	};

}