#include "Lpch.h"

#ifdef LAB_PLATFORM_LINUX
	#define STBI_NO_SIMD
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 