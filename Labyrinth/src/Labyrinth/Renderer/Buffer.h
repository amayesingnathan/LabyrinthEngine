#pragma once

#include <Labyrinth/Core/System/Ref.h>

namespace Laby {

	enum class ShaderDataType : u8
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static u32 ShaderDataTypeSize(ShaderDataType type)
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
		std::string name;
		ShaderDataType type;
		usize offset;
		u32 size;
		bool normalised;

		BufferElement() = default;
		BufferElement(ShaderDataType t, const std::string& n, bool norm = false)
			: name(n), type(t), size(ShaderDataTypeSize(t)), offset(0), normalised(norm)
		{}

		u32 GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Mat3:		return 3;
			case ShaderDataType::Mat4:		return 4;
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
		BufferLayout(std::initializer_list<BufferElement> elements)
			: mElements(elements)
		{
			CalculateOffsetsAndStride();
		}

		const std::vector<BufferElement>& GetElements() const { return mElements; }
		u32 GetStride() const { return mStride; }

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
			usize offset = 0;
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
		u32 mStride = 0;
	};

	class VertexBuffer : public virtual RefCounted
	{
	public:
		VertexBuffer(u32 size);
		VertexBuffer(f32* vertices, u32 size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, u32 size);

		const BufferLayout& GetLayout() const { return mLayout; }
		void SetLayout(const BufferLayout& layout) { mLayout = layout; }

	private:
		u32 mRendererID;
		BufferLayout mLayout;
	};

	class IndexBuffer : public virtual RefCounted
	{
	public:
		IndexBuffer(u32* indices, u32 count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		u32 GetCount() const { return mCount; }

	private:
		u32 mRendererID;
		u32 mCount;
	};

}