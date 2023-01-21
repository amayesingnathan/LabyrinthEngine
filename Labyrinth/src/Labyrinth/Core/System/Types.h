#pragma once

#include <cstdint>

#include <glm/glm.hpp>

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

	template<typename T>
	struct Limits
	{
		static constexpr T Eps = std::numeric_limits<T>::epsilon();
		static constexpr T Min = std::numeric_limits<T>::min();
		static constexpr T Max = std::numeric_limits<T>::max();
	};

	struct Math
	{
		static constexpr f64 e = 2.71828182845904523536;
		static constexpr f64 pi = 3.14159265358979323846;
		static constexpr f64 pi_2 = 1.57079632679489661923;
		static constexpr f64 pi_3 = 1.04719755119659774615;
		static constexpr f64 pi_4 = 0.785398163397448309616;
		static constexpr f64 one_pi = 0.318309886183790671538;
		static constexpr f64 two_pi = 0.318309886183790671538;
		static constexpr f64 sqrt2 = 1.41421356237309504880;
		static constexpr f64 one_sqrt2 = 0.707106781186547524401;
	};

	struct Angle
	{
		static constexpr f64 Rad5 = 0.0872664625997164788461845;
		static constexpr f64 Rad10 = 0.174532925199432957692369;
		static constexpr f64 Rad20 = 0.261799387799149436538554;
		static constexpr f64 Rad30 = 0.523598775598298873077107;
		static constexpr f64 Rad45 = Math::pi_4;
		static constexpr f64 Rad60 = Math::pi_3;
		static constexpr f64 Rad90 = Math::pi_2;
		static constexpr f64 Rad120 = 2.09343951023931954923084;
		static constexpr f64 Rad180 = Math::pi;
		static constexpr f64 Rad270 = 4.712388980384679857694;
		static constexpr f64 Rad360 = 6.28318530717958647692529;

		static constexpr f32 Rad5f = StaticCast<f32>(Rad5);
		static constexpr f32 Rad10f = StaticCast<f32>(Rad10);
		static constexpr f32 Rad20f = StaticCast<f32>(Rad20);
		static constexpr f32 Rad30f = StaticCast<f32>(Rad30);
		static constexpr f32 Rad45f = StaticCast<f32>(Rad45);
		static constexpr f32 Rad60f = StaticCast<f32>(Rad60);
		static constexpr f32 Rad90f = StaticCast<f32>(Rad90);
		static constexpr f32 Rad120f = StaticCast<f32>(Rad120);
		static constexpr f32 Rad180f = StaticCast<f32>(Rad180);
		static constexpr f32 Rad270f = StaticCast<f32>(Rad270);
		static constexpr f32 Rad360f = StaticCast<f32>(Rad360);
	};

	struct Zero
	{
		static constexpr glm::vec2 Vec2 = glm::vec2{ 0.0f };
		static constexpr glm::vec3 Vec3 = glm::vec3{ 0.0f };

		static constexpr glm::mat2 Mat2 = glm::mat2{ 0.0f };
		static constexpr glm::mat3 Mat3 = glm::mat3{ 0.0f };
		static constexpr glm::mat4 Mat4 = glm::mat4{ 0.0f };
	};

}