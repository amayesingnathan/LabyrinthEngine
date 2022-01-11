#pragma once

#include "Labyrinth/Core/Base.h"
#include "Labyrinth/Core/Log.h"
#include <filesystem>

#ifdef LAB_ENABLE_ASSERTS
	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define LAB_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { LAB##type##ERROR(msg, __VA_ARGS__); LAB_DEBUGBREAK(); } }
	#define LAB_INTERNAL_ASSERT_WITH_MSG(type, check, ...) LAB_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define LAB_INTERNAL_ASSERT_NO_MSG(type, check) LAB_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", LAB_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define LAB_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define LAB_INTERNAL_ASSERT_GET_MACRO(...) LAB_EXPAND_MACRO( LAB_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, LAB_INTERNAL_ASSERT_WITH_MSG, LAB_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define LAB_ASSERT(...) LAB_EXPAND_MACRO( LAB_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define LAB_CORE_ASSERT(...) LAB_EXPAND_MACRO( LAB_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define LAB_ASSERT(...)
	#define LAB_CORE_ASSERT(...)
#endif