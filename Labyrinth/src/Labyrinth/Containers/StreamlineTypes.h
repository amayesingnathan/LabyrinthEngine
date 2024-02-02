#pragma once

#include "Types/Buffer.h"
#include "Types/StaticString.h"

namespace Laby {

	using Buffer = slc::Buffer;

	template<size_t TSize>
	using StaticBuffer = slc::StaticBuffer<TSize>;
	template<size_t TSize>
	using StaticString= slc::StaticString<TSize>;
}