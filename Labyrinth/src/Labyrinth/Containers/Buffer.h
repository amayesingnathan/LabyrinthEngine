#pragma once

#include <Labyrinth/Core/System/Assert.h>

namespace Laby {

	class Buffer
	{
	public:
		Buffer() = default;
		Buffer(std::nullptr_t) {}
		Buffer(void* data, usize size) : mData(data), mSize(size) {}
		Buffer(usize size) { allocate(size); }

		Buffer(const Buffer& buffer);
		Buffer(Buffer&& buffer) noexcept;

		virtual ~Buffer() { release(); }

		Buffer& operator=(const Buffer& buffer);
		Buffer& operator=(Buffer&& buffer) noexcept;

		static Buffer Copy(const void* data, usize size);

	private:
		void allocate(usize size);
		void release();

	public:
		template<typename T>
		T& read(usize offset = 0) { return *(T*)((byte*)mData + offset); }
		template<typename T>
		const T& read(usize offset = 0) const { return *(T*)((byte*)mData + offset); }

		template<typename T>
		T* as() const { return (T*)mData; }

		Buffer copyBytes(usize size, usize offset = 0);

	public:
		operator bool() const { return mData != nullptr; }

		Byte& operator[](usize index) { return ((Byte*)mData)[index]; }
		const Byte& operator[](usize index) const { return ((Byte*)mData)[index]; }

	private:
		void* mData = nullptr;
		usize mSize = 0;
	};
}