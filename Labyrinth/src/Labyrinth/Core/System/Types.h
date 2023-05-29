#pragma once

#include <cstdint>

#include "Cast.h"

namespace Laby {

	using i8  = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using u8  = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using usize = size_t;

	using f32 = float;
	using f64 = double;

	using Byte = u8;

	template<typename TResult, typename... TArgs>
	using Func = std::function<TResult(TArgs...)>;

	template<typename... TArgs>
	using Action = Func<void, TArgs...>;

	template<typename... T>
	using Predicate = Func<bool, T...>;
}