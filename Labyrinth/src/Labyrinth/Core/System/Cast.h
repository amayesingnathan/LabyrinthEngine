#pragma once

#include "Assert.h"
#include "Reflection.h"

namespace Laby {

	template<typename T>
	constexpr T* StaticCast(void* target)
	{
		return static_cast<T*>(target);
	}
	template<typename To, typename From>
	constexpr To StaticCast(From target)
	{
		return static_cast<To>(target);
	}
	template<typename To, typename From>
	constexpr To* StaticCast(From* target)
	{
		return static_cast<To*>(target);
	}

	template<typename To, typename From>
	constexpr To* DynCast(From* target)
	{
		LAB_STATIC_ASSERT(TypeTraits<From>::IsBaseOf<To> || TypeTraits<To>::IsBaseOf<From>);
		return dynamic_cast<To*>(target);
	}
}