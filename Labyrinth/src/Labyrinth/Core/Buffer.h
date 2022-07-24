#pragma once

#include <Labyrinth/Core/System/Assert.h>

namespace Labyrinth {

	using byte = u8;

	struct Buffer
	{
		void* data = nullptr;
		usize size = 0;

		Buffer() = default;
		Buffer(void* _data, usize _size) : data(_data), size(_size) {}
		Buffer(const Buffer& buffer)
		{
			LAB_CORE_ASSERT(buffer.data && buffer.size);

			allocate(buffer.size);
			memcpy(data, buffer.data, buffer.size);
		}

		static Buffer Copy(const void* _data, usize size)
		{
			Buffer buffer;
			buffer.allocate(size);
			memcpy(buffer.data, _data, size);
			return buffer;
		}

		Buffer& operator=(const Buffer& buffer)
		{
			LAB_CORE_ASSERT(buffer.data && buffer.size);

			allocate(buffer.size);
			memcpy(data, buffer.data, buffer.size);
		}

		void allocate(usize _size)
		{
			delete[](byte*)data;
			data = nullptr;

			if (_size == 0)
				return;

			data = new byte[_size];
			size = _size;
		}

		void free()
		{
			delete[](byte*)data;
			data = nullptr;
			size = 0;
		}

		template<typename T>
		T& read(usize _offset = 0)
		{
			return *(T*)((byte*)data + offset);
		}

		Buffer copyBytes(usize _size, usize _offset = 0)
		{
			LAB_CORE_ASSERT(_offset + _size <= size, "Buffer overflow!");
			return Buffer::Copy((byte*)data + _offset, _size);
		}

		void append(const void* _data, usize _size, usize _offset = 0)
		{
			LAB_CORE_ASSERT(_offset + _size <= size, "Buffer overflow!");
			memcpy((byte*)data + _offset, _data, _size);
		}

		operator bool() const
		{
			return data;
		}

		byte& operator[](usize index)
		{
			return ((byte*)data)[index];
		}

		byte operator[](usize index) const
		{
			return ((byte*)data)[index];
		}

		template<typename T>
		T* as() const
		{
			return (T*)data;
		}

		inline usize size() const { return size; }
	};

	struct ScopedBuffer : public Buffer
	{
		~ScopedBuffer()
		{
			free();
		}

		static ScopedBuffer Copy(const void* _data, usize size)
		{
			ScopedBuffer buffer;
			buffer.allocate(size);
			memcpy(buffer.data, _data, size);
			return buffer;
		}
	};
}