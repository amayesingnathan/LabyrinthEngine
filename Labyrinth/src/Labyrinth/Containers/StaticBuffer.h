#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	template<usize TSize>
	class StaticBuffer
	{
	public:
		StaticBuffer() = default;
		StaticBuffer(Byte data[TSize])
		{
			memset(mData, 0, TSize);
			memcpy(mData, data, TSize);
		}

		template<usize _Other>
		StaticBuffer(const StaticBuffer<_Other>& buffer)
		{
			memset(mData, 0, TSize);
			if constexpr (_Other <= TSize)
				memcpy(mData, buffer.mData, _Other);
			else
				memcpy(mData, buffer.mData, TSize);
		}

		Byte& operator[](usize index)
		{
			return mData[index];
		}

		Byte operator[](usize index) const
		{
			return mData[index];
		}

		constexpr usize size() const { return TSize; }

	protected:
		Byte mData[TSize] = { 0 };
	};
}