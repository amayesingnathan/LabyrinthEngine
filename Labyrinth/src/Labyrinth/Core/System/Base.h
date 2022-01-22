	#pragma once

#include <memory>
#include "Labyrinth/Core/System/Platforms.h"

#ifdef LAB_DEBUG
	#if defined(LAB_PLATFORM_WINDOWS)
		#define LAB_DEBUGBREAK() __debugbreak()
	#elif defined(HZ_PLATFORM_LINUX)
		#include <signal.h>
		#define LAB_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define LAB_ENABLE_ASSERTS
#else
	#define LAB_DEBUGBREAK()
#endif


#define LAB_EXPAND_MACRO(x) x
#define LAB_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define LAB_BIND_EVENT_FUNC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Labyrinth {

	/*
		Smart Pointer Tools
	*/

	template<typename T>
	using Single = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Single<T> CreateSingle(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
	template<typename T>
	constexpr Ref<T> CloneRef(const T& copy)
	{
		return std::make_shared<T>(copy);
	}

}