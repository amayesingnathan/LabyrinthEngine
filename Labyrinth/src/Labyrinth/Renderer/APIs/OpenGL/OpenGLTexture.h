#pragma once

#include "Labyrinth/Renderer/Texture.h"

#include <glad/glad.h>

namespace Labyrinth {

	class LAB_API OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(size_t width, size_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual const std::string& getPath() const override { return mPath; }
		virtual size_t getWidth() const override { return mWidth; }
		virtual size_t getHeight() const override { return mHeight; }
		virtual uint32_t getRendererID() const override { return mRendererID; }

		virtual void setData(void* data, size_t size) override;

		virtual void bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return mRendererID == other.getRendererID();
		}

	private:
		std::string mPath;
		size_t mWidth, mHeight;
		uint32_t mRendererID;
		GLenum mInternalFormat, mDataFormat;
	};

}