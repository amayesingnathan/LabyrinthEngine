#include "Lpch.h"
#include "Buffer.h"

namespace Laby {

	Buffer::Buffer(const Buffer& buffer)
	{
		LAB_CORE_ASSERT(buffer.mData && buffer.mSize);

		allocate(buffer.mSize);
		memcpy(mData, buffer.mData, buffer.mSize);
	}

	Buffer::Buffer(Buffer&& buffer) noexcept
	{
		if (&buffer == this) 
			return;

		mData = buffer.mData;
		buffer.mData = nullptr;

		mSize = buffer.mSize;
	}

	Buffer& Buffer::operator=(const Buffer& buffer)
	{
		LAB_CORE_ASSERT(buffer.mData && buffer.mSize);

		allocate(buffer.mSize);
		memcpy(mData, buffer.mData, buffer.mSize);

		return *this;
	}

	Buffer& Buffer::operator=(Buffer&& buffer) noexcept
	{
		LAB_CORE_ASSERT(&buffer != this, "Cannot move assign an object to itself!");

		mData = buffer.mData;
		buffer.mData = nullptr;

		mSize = buffer.mSize;

		return *this;
	}

	Buffer Buffer::Copy(const void* data, usize size)
	{
		return Buffer();
	}

	void Buffer::allocate(usize size)
	{
		release();

		if (size == 0)
			return;

		mData = new Byte[size];
		mSize = size;
	}

	void Buffer::release()
	{
		delete[](Byte*)mData;
		mData = nullptr;
		mSize = 0;
	}

	Buffer Buffer::copyBytes(usize size, usize offset)
	{
		LAB_CORE_ASSERT(offset + size <= mSize, "Buffer overflow!");
		return Buffer::Copy((Byte*)mData + offset, size);
	}

	void Buffer::resize(usize newSize)
	{
		if (mSize == newSize)
			return;

		Byte* newData = new Byte[newSize];
		memcpy(newData, mData, (newSize > mSize) ? mSize : newSize);
		delete[] mData;

		mData = newData;
		mSize = newSize;
	}
}