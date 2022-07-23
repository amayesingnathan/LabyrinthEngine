#pragma once

#include "Labyrinth/Core/System/Base.h"

namespace Labyrinth {

	class UniformBuffer : public RefCounted
	{
	public:
		virtual ~UniformBuffer() {}
		virtual void setData(const void* data, u32 size, u32 offset = 0) = 0;

		static Ref<UniformBuffer> Create(u32 size, u32 binding);
	};

}