#include "Lpch.h"
#include "Texture.h"

#include <glad/glad.h>

#include "stb_image.h"

namespace Laby {

	Texture2D::Texture2D(i32 width, i32 height)
		: mWidth(width), mHeight(height)
	{
		mInternalFormat = GL_RGBA8;
		mDataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
		glTextureStorage2D(mRendererID, 1, mInternalFormat, mWidth, mHeight);

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	Texture2D::Texture2D(const std::string& path)
	{
		i32 width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		u8* data = nullptr;
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (stbi_failure_reason())
			LAB_CORE_TRACE("{0}", stbi_failure_reason());
		LAB_CORE_ASSERT(data, "Failed to load image!");

		mWidth = width;
		mHeight = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		mInternalFormat = internalFormat;
		mDataFormat = dataFormat;

		LAB_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
		glTextureStorage2D(mRendererID, 1, internalFormat, mWidth, mHeight);

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &mRendererID);
	}

	u32 Texture2D::getSize() const
	{
		u32 bpp = mDataFormat == GL_RGBA ? 4 : 3;
		return mWidth * mHeight * bpp;
	}

	void Texture2D::setData(void* data, usize size)
	{
		u32 bpp = mDataFormat == GL_RGBA ? 4 : 3;
		LAB_CORE_ASSERT(size == mWidth * mHeight * bpp, "Data must be entire texture!");
		glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
	}

	void Texture2D::setData(Buffer buffer)
	{
		u32 bpp = mDataFormat == GL_RGBA ? 4 : 3;
		LAB_CORE_ASSERT(buffer.size == mWidth * mHeight * bpp, "Data must be entire texture!");
		glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, buffer.data());
	}

	Buffer Texture2D::getData()
	{
		u32 bpp = mDataFormat == GL_RGBA ? 4 : 3;
		i32 size = mWidth * mHeight * bpp;

		Buffer buffer(size);
		glGetTextureImage(mRendererID, GL_TEXTURE_2D, mDataFormat, GL_UNSIGNED_BYTE, size, buffer.data());
		return buffer;
	}
}