#pragma once

#include "Buffer.h"

namespace Laby {

	class VertexArray : public virtual RefCounted
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		virtual void bind() const;
		virtual void unbind() const;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return mVertexBuffers; }
		virtual const Ref<IndexBuffer>& getIndexBuffer() const { return mIndexBuffer; }

	private:
		u32 mRendererID;
		u32 mVertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> mVertexBuffers;
		Ref<IndexBuffer> mIndexBuffer;

	};

}