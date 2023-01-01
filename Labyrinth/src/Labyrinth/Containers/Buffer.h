#pragma once

#include <Labyrinth/Core/System/Base.h>

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

	public:
		template<typename T>
		T& read(usize offset = 0) { return *(T*)((Byte*)mData + offset); }
		template<typename T>
		const T& read(usize offset = 0) const { return *(T*)((Byte*)mData + offset); }

		template<typename T>
		T* as() const { return (T*)mData; }

		Byte* data() { return (Byte*)mData; }
		const Byte* data() const { return (Byte*)mData; }

		Buffer copyBytes(usize size, usize offset = 0);

		usize size() const { return mSize; }

	public:
		operator bool() const { return mData != nullptr; }

		Byte& operator[](usize index) { return ((Byte*)mData)[index]; }
		const Byte& operator[](usize index) const { return ((Byte*)mData)[index]; }

	private:
		void allocate(usize size);
		void release();

	private:
		void* mData = nullptr;
		usize mSize = 0;
	};
}