#pragma once

#include "Labyrinth/Renderer/Texture.h"

#include <glad/glad.h>

namespace Labyrinth {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t getWidth() const override { return mWidth; }
		virtual uint32_t getHeight() const override { return mHeight; }

		virtual void setData(void* data, uint32_t size) override;

		virtual void bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return mRendererID == ((OpenGLTexture2D&)other).mRendererID;
		}

	private:
		std::string mPath;
		uint32_t mWidth, mHeight;
		uint32_t mRendererID;
		GLenum mInternalFormat, mDataFormat;
	};

}