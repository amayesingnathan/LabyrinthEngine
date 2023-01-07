#pragma once

#include "StaticBuffer.h"

namespace Laby {

	template<usize TSize>
	class StaticString : public StaticBuffer<TSize>
	{
	public:
		StaticString(const std::string& string)
		{
			LAB_CORE_ASSERT(string.size() <= TSize);

			memset(this->mData, 0, TSize);
			memcpy(this->mData, string.c_str(), string.size());;
		}

		constexpr usize length() const { return TSize; }

		operator char*()
		{
			LAB_CORE_ASSERT(this->mData[TSize - 1] == 0); // At least the last character should be null
			return (char*)this->mData;
		}

		std::string toString() const
		{
			LAB_CORE_ASSERT(this->mData[TSize - 1] == 0); // At least the last character should be null
			return (const char*)this->mData;
		}
	};
}