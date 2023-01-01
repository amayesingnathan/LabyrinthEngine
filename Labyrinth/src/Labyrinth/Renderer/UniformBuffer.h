#pragma once

#include <Labyrinth/Core/System/Ref.h>

namespace Laby {

	class UniformBuffer : public virtual RefCounted
	{
	public:
		UniformBuffer(u32 size, u32 binding);
		virtual ~UniformBuffer();

		void setData(const void* data, u32 size, u32 offset = 0);

	private:
		u32 mRendererID = 0;
	};
}