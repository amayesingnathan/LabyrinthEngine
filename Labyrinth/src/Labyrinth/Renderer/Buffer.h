#pragma once

#include <cstdint>

namespace Labyrinth {

	enum class ShaderDataType : uint8_t
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
		}

		LAB_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		ShaderDataType type;
		std::string name;
		uint32_t offset;
		uint32_t size;
		bool normalised;

		BufferElement() = default;
		BufferElement(ShaderDataType t, const std::string& n, bool norm = false)
			: type(t), name(n), size(ShaderDataTypeSize(t)), offset(0), normalised(norm)
		{}

		uint32_t getComponentCount() const
		{
			switch (type)
			{
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Mat3:		return 3 * 3;
				case ShaderDataType::Mat4:		return 4 * 4;
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Bool:		return 1;
			}

			LAB_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: mElements(elements) 
		{
			CalculateOffsetsAndStride();
		}

		const std::vector<BufferElement>& getElements() const { return mElements; }
		uint32_t getStride() const { return mStride; }

		std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
		std::vector<BufferElement>::iterator end() { return mElements.end(); }
		std::vector<BufferElement>::reverse_iterator rbegin() { return mElements.rbegin(); }
		std::vector<BufferElement>::reverse_iterator rend() { return mElements.rend(); }

		std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
		std::vector<BufferElement>::const_iterator end()	const { return mElements.end(); }
		std::vector<BufferElement>::const_reverse_iterator rbegin() const { return mElements.rbegin(); }
		std::vector<BufferElement>::const_reverse_iterator rend() const { return mElements.rend(); }

	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			mStride = 0;
			for (auto& element : mElements)
			{
				element.offset = offset;
				offset += element.size;
				mStride += element.size;
			}
		}

	private:
		std::vector<BufferElement> mElements;
		uint32_t mStride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		virtual const BufferLayout& getLayout() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setData(const void* data, uint32_t count) = 0;

		virtual uint32_t getCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};
}