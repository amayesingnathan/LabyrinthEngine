#pragma once

#include <magic_enum.hpp>

namespace Laby::Enum {

	template<typename T>
	constexpr std::string_view ToString(T _enum)
	{
		LAB_STATIC_ASSERT(MAGIC_ENUM_SUPPORTED, "Compiler does not support magic enums! Define your own conversions!");
		LAB_STATIC_ASSERT(std::is_enum<T>::value, "Argument must be an enum!");

		return magic_enum::enum_name(_enum);
	}

	template<typename T>
	constexpr T FromString(std::string_view _enumStr)
	{
		LAB_STATIC_ASSERT(MAGIC_ENUM_SUPPORTED, "Compiler does not support magic enums! Define your own conversions!");

		auto _enum = magic_enum::enum_cast<T>(_enumStr);
		if (_enum.has_value())
			return _enum.value();

		return T::None;
	}
}