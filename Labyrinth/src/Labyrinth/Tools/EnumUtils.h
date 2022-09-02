#pragma once

#include <magic_enum.hpp>

namespace Labyrinth::Enum {

	template<typename T>
	inline std::string ToString(T _enum)
	{
		LAB_STATIC_ASSERT(MAGIC_ENUM_SUPPORTED, "Compiler does not support magic enums! Define your own conversions!");
		LAB_STATIC_ASSERT(std::is_enum<T>::value, "Argument must be an enum!");

		return std::string(magic_enum::enum_name(_enum));
	}

	template<typename T>
	inline T FromString(const std::string& _enumStr)
	{
		LAB_STATIC_ASSERT(MAGIC_ENUM_SUPPORTED, "Compiler does not support magic enums! Define your own conversions!");

		auto _enum = magic_enum::enum_cast<T>(_enumStr);
		if (_enum.has_value()) 
			return _enum.value();

		return T::None;
	}
}