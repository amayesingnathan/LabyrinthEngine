#pragma once

#include <memory>

#include "Platform.h"
#include "System.h"
#include "Cast.h"
#include "Types.h"
#include "Constants.h"

#define LABYRINTH_VER 0xABBA011

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

#define LAB_BIT(x) (1 << x)

#define LAB_BIND_EVENT_FUNC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Laby {

	/*
		Smart Pointer Tools
	*/

	template<typename T>
	using Single = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr inline Single<T> MakeSingle(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

}