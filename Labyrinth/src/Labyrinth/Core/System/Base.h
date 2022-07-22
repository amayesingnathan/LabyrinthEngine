#pragma once

#define LABYRINTH_VER 1.0

#include <memory>
#include "Labyrinth/Core/System/API.h"
#include <Labyrinth/Core/System/Types.h>

#ifdef LAB_DEBUG
	#if defined(LAB_PLATFORM_WINDOWS)
		#define LAB_DEBUGBREAK() __debugbreak()
	#elif defined(LAB_PLATFORM_LINUX)
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
	constexpr inline Single<T> CreateSingle(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using AllowRefFromThis = std::enable_shared_from_this<T>;

	template<typename T, typename ... Args>
	constexpr inline Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
	// Object T must inherit from AllowRefFromThis<T>
	template<typename T>
	constexpr inline Ref<T> CreateRef(T* raw_ptr)
	{
		return raw_ptr->shared_from_this(); 
	}

	template<typename T>
	using WeakRef = std::weak_ptr<T>;
}