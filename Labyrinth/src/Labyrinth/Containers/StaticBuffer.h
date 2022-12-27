#pragma once

#include <Labyrinth/Core/System/Types.h>

namespace Laby {

	template<usize _Size>
	class StaticBuffer
	{
	public:
		StaticBuffer() = default;
		StaticBuffer(Byte data[_Size])
		{
			memset(mData, 0, _Size);
			memcpy(mData, data, _Size);
		}

		template<usize _Other>
		StaticBuffer(const StaticBuffer<_Other>& buffer)
		{
			memset(mData, 0, _Size);
			if constexpr (_Other <= _Size)
				memcpy(mData, buffer.mData, _Other);
			else
				memcpy(mData, buffer.mData, _Size);
		}

		Byte& operator[](usize index)
		{
			return mData[index];
		}

		Byte operator[](usize index) const
		{
			return mData[index];
		}

		constexpr usize size() const { return _Size; }

	protected:
		Byte mData[_Size] = { 0 };
	};
}