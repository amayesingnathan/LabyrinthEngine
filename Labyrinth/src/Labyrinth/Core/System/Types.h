#pragma once

#include <cstdint>

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

	struct Limits
	{
		static constexpr i32 i8Min = -127;
		static constexpr i32 i8Max = -127;
		static constexpr i32 i16Min = -32768;
		static constexpr i32 i16Max = 32767;
		static constexpr i32 u8Max = 255;
		static constexpr i32 u16Max = 65535;
	};

}