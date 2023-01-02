#pragma once

#include "Texture.h"

#include <Labyrinth/Assets/Asset.h>

#include <map>

namespace Laby {

	class SubTexture2D;

	//Texture2DSheet is a thin wrapper around a Texture2D for a sprite sheet which contains meta data about the sheet.
	class Texture2DSheet : public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::TextureSheet)

	public:
		Texture2DSheet(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name = "");
		Texture2DSheet(const std::string& filepath, const glm::vec2& tileSize, const std::string& name = "");

		operator Ref<Texture2D>() const { return mTexture; }

		usize getWidth() const { return mTexture->getWidth(); }
		usize getHeight() const { return mTexture->getHeight(); }

		u32 getTileCountX() const { return mTileCountX; }
		u32 getTileCountY() const { return mTileCountY; }

		glm::vec2 getTileSize() const { return mTileSize; }
		u32 getTileWidth() const { return StaticCast<u32>(mTileSize.x); }
		u32 getTileHeight() const { return StaticCast<u32>(mTileSize.y); }

		glm::vec2 getTileSizeN() const { return { mTileSize.x / mTexture->getWidth(), mTileSize.y / mTexture->getHeight() }; }

		const std::string& getName() const { return mName; }
		const Ref<Texture2D>& getBaseTex() const { return mTexture; }

		bool hasSubTex(i32 id) const;
		AssetHandle getSubTex(i32 id);

		std::map<i32, AssetHandle>& getSubTexList() { return mSubTextures; }
		const std::map<i32, AssetHandle>& getSubTexList() const { return mSubTextures; }

		void addSubTex(i32 id, AssetHandle handle);
		AssetHandle createSubTex(i32 id, const std::string& name, const glm::vec2& coords, const glm::vec2& spriteSize = glm::vec2(1.f));
		AssetHandle createSubTex(i32 id, const std::string& name, const glm::vec2 coords[4]);
		void deleteSubTex(i32 id);

		void generateTileset(i32 startIndex = 0);
		void clearTileset();

		AssetHandle operator[] (i32 id) const;

	private:
		std::string mName;
		Ref<Texture2D> mTexture;
		glm::vec2 mTileSize;
		u32 mTileCountX, mTileCountY;
		std::map<i32, AssetHandle> mSubTextures;

		friend class SubTexture2D;
	};

	class SubTexture2D : public IRenderable, public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::SubTexture)

	public:
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2& coords, const glm::vec2& spriteSize, const std::string& name);
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2 coords[4], const std::string& name);
		SubTexture2D(const Ref<Texture2D>& sheet, const std::string& name = "");

		u32 getRendererID() const override { return mSheet->mTexture->getRendererID(); }
		void bind(u32 slot = 0) const override { return mSheet->mTexture->bind(); }
		const glm::vec2* getTextureCoords() const override { return mTexCoords; }

		const std::string& getName() const { return mName; }

		Ref<Texture2DSheet> getSheet() const { return mSheet; }
		Ref<Texture2D> getBaseTex() const { return mSheet->mTexture; }

	private:
		std::string mName;
		Ref<Texture2DSheet> mSheet;
		glm::vec2 mTexCoords[4];
	};

}