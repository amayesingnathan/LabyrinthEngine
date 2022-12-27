#pragma once

#include "StaticBuffer.h"

namespace Laby {

	template<usize _Size>
	class StaticString : private StaticBuffer<_Size>
	{
	public:
		StaticString(const std::string& string)
		{
			LAB_CORE_ASSERT(string.size() <= _Size);

			memset(mData, 0, _Size);
			memcpy(mData, string.c_str(), string.size());;
		}

		constexpr usize length() const { return _Size; }

		operator char* ()
		{
			LAB_CORE_ASSERT(mData[_Size - 1] == 0); // At least the last character should be null
			return (char*)mData;
		}

		std::string toString() const
		{
			LAB_CORE_ASSERT(mData[_Size - 1] == 0); // At least the last character should be null
			return (const char*)mData;
		}
	};
}