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
		Buffer(usize _size) { allocate(_size); }
		Buffer(const Buffer& buffer)
		{
			LAB_CORE_ASSERT(buffer.data && buffer.size);

			allocate(buffer.size);
			memcpy(data, buffer.data, buffer.size);
		}
		Buffer(Buffer&& buffer)
		{
			if (&buffer == this) return;

			data = buffer.data;
			buffer.data = nullptr;

			size = buffer.size;
		}
		virtual ~Buffer() { release(); }

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

			return *this;
		}
		Buffer& operator=(Buffer&& buffer)
		{
			LAB_CORE_ASSERT(&buffer != this, "Cannot move assign an object to itself!");

			data = buffer.data;
			buffer.data = nullptr;

			size = buffer.size;

			return *this;
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

		void release()
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
	};

	template<usize _Size>
	struct StaticBuffer
	{
		byte data[_Size] = { 0 };

		StaticBuffer() = default;
		StaticBuffer(byte _data[_Size])
		{
			memset(data, 0, _Size);
			memcpy(data, _data, _Size);
		}
		StaticBuffer(const std::string& string)
		{
			LAB_CORE_ASSERT(string.size() <= _Size);

			memset(data, 0, _Size);
			memcpy(data, string.c_str(), string.size());;
		}

		template<usize _Other>
		StaticBuffer(const StaticBuffer<_Other>& buffer)
		{
			memset(data, 0, _Size);
			if constexpr (_Other <= _Size)
				memcpy(data, buffer.data, _Other);
			else
				memcpy(data, buffer.data, _Size);
		}

		operator char*() 
		{
			LAB_CORE_ASSERT(data[_Size - 1] == 0); // At least the last character should be null
			return (char*)data;
		}

		byte& operator[](usize index)
		{
			return data[index];
		}

		byte operator[](usize index) const
		{
			return data[index];
		}

		const char* string() const 
		{
			LAB_CORE_ASSERT(data[_Size - 1] == 0); // At least the last character should be null
			return (const char*)data;
		}

	};
}