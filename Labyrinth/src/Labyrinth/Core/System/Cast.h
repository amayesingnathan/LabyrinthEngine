#pragma once

#include "Assert.h"
#include "Reflection.h"

namespace Laby {

	/*
		Unique Pointer
	*/

	template<typename T>
	using Single = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	Single<T> MakeSingle(Args&& ... args) { return std::make_unique<T>(std::forward<Args>(args)...); }



	/*
		Casting Tools
	*/

	template<typename T>
	constexpr T* Cast(void* target)
	{
		return static_cast<T*>(target);
	}
	template<typename To, typename From>
	constexpr To Cast(From target)
	{
		return static_cast<To>(target);
	}
	template<typename To, typename From>
	constexpr To* Cast(From* target)
	{
		return static_cast<To*>(target);
	}

	template<typename To, typename From>
	constexpr To* Cast(From* targetPointer)
	{
		LAB_STATIC_ASSERT(TypeTraits<To>::IsBaseOf<From> || TypeTraits<From>::IsBaseOf<To>);
		return dynamic_cast<To*>(targetPointer);
	}
}