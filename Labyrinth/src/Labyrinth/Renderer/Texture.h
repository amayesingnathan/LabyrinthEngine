#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Containers/Buffer.h>

#include "IRenderable.h"

typedef unsigned int GLenum;

namespace Laby {

	class Texture2D : public IRenderable, public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::Texture)
		
	public:
		Texture2D(i32 width, i32 height);
		Texture2D(const std::string& path);
		~Texture2D();

		bool loaded() const { return mRendererID != -1; }

		u32 getTextureID() const override { return mRendererID; }
		void bindTexture(u32 slot = 0) const override;

		i32 getWidth() const { return mWidth; }
		i32 getHeight() const { return mHeight; }
		u32 getSize() const;

		void setData(void* data, usize size);
		void setData(Buffer buffer);
		Buffer getData();

	private:
		i32 mWidth, mHeight;
		u32 mRendererID = -1;
		GLenum mInternalFormat, mDataFormat;
	};

}