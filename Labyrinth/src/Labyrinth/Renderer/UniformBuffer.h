#pragma once

#include "Labyrinth/Core/System/Base.h"

namespace Labyrinth {

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}
		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};

}