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
		Texture2DSheet(const std::string& name, const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize);
		Texture2DSheet(const std::string& name, const std::string& filepath, const glm::vec2& tileSize);

		operator Ref<Texture2D>() const { return mTexture; }

		const std::string& getName() const { return mName; }

		usize getWidth() const { return mTexture->getWidth(); }
		usize getHeight() const { return mTexture->getHeight(); }

		u32 getTileCountX() const { return mTileCountX; }
		u32 getTileCountY() const { return mTileCountY; }

		glm::vec2 getTileSize() const { return mTileSize; }
		u32 getTileWidth() const { return StaticCast<u32>(mTileSize.x); }
		u32 getTileHeight() const { return StaticCast<u32>(mTileSize.y); }

		glm::vec2 getTileSizeN() const { return { mTileSize.x / mTexture->getWidth(), mTileSize.y / mTexture->getHeight() }; }

		const Ref<Texture2D>& getBaseTex() const { return mTexture; }

		bool hasSubTex(AssetHandle id) const;
		AssetHandle getSubTex(AssetHandle id);

		void destroyTileset();

		const std::unordered_set<AssetHandle>& getSubTextures() const { return mSubTextures; }

	private:
		void GenerateTileset();

		AssetHandle CreateSubTex(usize index, const glm::vec2& coords, const glm::vec2& spriteSize = glm::vec2(1.f));
		AssetHandle CreateSubTex(usize index, const glm::vec2 coords[4]);

	private:
		std::string mName;
		Ref<Texture2D> mTexture;
		glm::vec2 mTileSize;
		u32 mTileCountX, mTileCountY;
		std::unordered_set<AssetHandle> mSubTextures;

		friend class SubTexture2D;
	};

	class SubTexture2D : public IRenderable, public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::SubTexture)

	public:
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2& coords, const glm::vec2& spriteSize);
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2 coords[4]);
		SubTexture2D(const Ref<Texture2D>& sheet);
		~SubTexture2D();

		u32 getTextureID() const override { return mSheet->mTexture->getTextureID(); }
		void bindTexture(u32 slot = 0) const override { return mSheet->mTexture->bindTexture(); }
		const glm::vec2* getTextureCoords() const override { return mTexCoords; }

		Ref<Texture2DSheet> getSheet() const { return mSheet; }
		Ref<Texture2D> getBaseTex() const { return mSheet->mTexture; }

	private:
		Ref<Texture2DSheet> mSheet;
		glm::vec2 mTexCoords[4];
	};

}