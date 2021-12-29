#pragma once

#ifdef LAB_PLATFORM_WINDOWS
	#ifdef LAB_BUILD_DLL
		#define LABYRINTH_API __declspec(dllexport)
	#else
		#define LABYRINTH_API __declspec(dllimport)

	#endif // LAB_BUILD_DLL
#else
	#error Labyrinth only supports Windows!
#endif // LAB_PLATFORM_WINDOWS
