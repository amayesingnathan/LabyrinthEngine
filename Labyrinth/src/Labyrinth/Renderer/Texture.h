#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Containers/StreamlineTypes.h>

#include "IRenderable.h"

typedef unsigned int GLenum;

namespace Laby {

	class Texture2D : public IRenderable, public Asset
	{
	public:
		ASSET_METADATA(AssetType::Texture, "textures")
		
	public:
		Texture2D(i32 width, i32 height);
		Texture2D(const std::string& path);
		~Texture2D();

		bool Loaded() const { return mRendererID != -1; }

		u32 GetTextureID() const override { return mRendererID; }

		i32 GetWidth() const { return mWidth; }
		i32 GetHeight() const { return mHeight; }
		u32 GetSize() const;

		void SetData(void* data, usize size);
		void SetData(Buffer buffer);
		Buffer GetData();

	private:
		i32 mWidth, mHeight;
		u32 mRendererID = -1;
		GLenum mInternalFormat, mDataFormat;
	};

}