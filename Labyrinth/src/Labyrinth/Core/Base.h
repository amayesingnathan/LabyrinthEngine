#pragma once

#include <memory>
#include "Labyrinth/Core/Platforms.h"

#ifdef LAB_PLATFORM_WINDOWS
	#ifdef LAB_BUILD_DLL
		#define LABYRINTH_API __declspec(dllexport)
	#else
		#define LABYRINTH_API __declspec(dllimport)

	#endif // LAB_BUILD_DLL
#else
	#error Labyrinth only supports Windows!
#endif // LAB_PLATFORM_WINDOWS


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

#define LAB_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
