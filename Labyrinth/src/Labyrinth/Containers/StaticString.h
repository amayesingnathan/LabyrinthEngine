#pragma once

#include "Buffer.h"

namespace Labyrinth {

	template<usize _Size>
	struct StaticString : private StaticBuffer<_Size>
	{
		StaticString(const std::string& string)
		{
			LAB_CORE_ASSERT(string.size() <= _Size);

			memset(data, 0, _Size);
			memcpy(data, string.c_str(), string.size());;
		}

		constexpr usize length() const { return _Size; }

		operator char* ()
		{
			LAB_CORE_ASSERT(data[_Size - 1] == 0); // At least the last character should be null
			return (char*)data;
		}

		std::string toString() const
		{
			LAB_CORE_ASSERT(data[_Size - 1] == 0); // At least the last character should be null
			return (const char*)data;
		}
	};
}