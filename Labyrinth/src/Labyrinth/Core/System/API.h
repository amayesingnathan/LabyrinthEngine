#pragma once

#include <Labyrinth/Core/System/Platforms.h>

#ifdef LAB_SHARED
	#ifdef LAB_PLATFORM_WINDOWS
		#ifdef LAB_EXPORTS
			#ifdef __GNUC__
				#define LAB_API __attribute__ ((dllexport))
			#else
				#define LAB_API __declspec(dllexport)
			#endif
		#else
			#ifdef __GNUC__
				#define LAB_API __attribute__ ((dllimport))
			#else
				#define LAB_API __declspec(dllimport)
			#endif
		#endif
	#elif defined(LAB_PLATFORM_LINUX)
		#if __GNUC__ >= 4
			#define LAB_API __attribute__ ((visibility ("default")))
		#else
			#define LAB_API
		#endif
	#endif
#else
	#define LAB_API
#endif