#pragma once

#include <array>

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	struct IComboEntry
	{
		std::string_view key;

		constexpr IComboEntry(std::string_view label) : key(label) {}
		virtual const void* getVal() const = 0;
	};

	template<typename T>
	struct ComboEntry : public IComboEntry
	{
		T value;

		constexpr ComboEntry(std::string_view label, const T& val)
			: IComboEntry(label), value(val) {}

		virtual const void* getVal() const { return &value; }
	};
}