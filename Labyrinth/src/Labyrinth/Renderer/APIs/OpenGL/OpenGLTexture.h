#pragma once

#include "Labyrinth/Renderer/Texture.h"

#include <glad/glad.h>

namespace Labyrinth {

	class LAB_API OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(i32 width, i32 height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual const std::string& getPath() const override { return mPath; }
		virtual i32 getWidth() const override { return mWidth; }
		virtual i32 getHeight() const override { return mHeight; }
		virtual u32 getRendererID() const override { return mRendererID; }

		virtual void setData(void* data, i32 size) override;

		virtual void bind(u32 slot = 0) const override;

		virtual bool loaded() const override { return mRendererID != -1; }

		virtual bool operator==(const Texture& other) const override
		{
			return mRendererID == other.getRendererID();
		}

	private:
		std::string mPath;
		i32 mWidth, mHeight;
		u32 mRendererID = -1;
		GLenum mInternalFormat, mDataFormat;
	};

}