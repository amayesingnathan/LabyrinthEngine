#pragma once

#include "Texture.h"

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Containers/Grid.h>

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

		const std::string& getName() const { return mName; }
		const Ref<Texture2D>& getBaseTex() const { return mTexture; }

		usize getWidth() const { return mTexture->getWidth(); }
		usize getHeight() const { return mTexture->getHeight(); }

		u32 getTileCountX() const { return mTileCountX; }
		u32 getTileCountY() const { return mTileCountY; }

		const glm::vec2& getTileSize() const { return mTileSize; }

		void generateTileset();
		void destroyTileset();

		const std::unordered_map<GridPosition, AssetHandle>& getSubTextures() const { return mSubTextures; }

	private:
		void CreateSubTex(usize index, const GridPosition& coords, const glm::vec2& spriteSize = glm::vec2(1.f));

	private:
		std::string mName;
		Ref<Texture2D> mTexture;
		glm::vec2 mTileSize;
		u32 mTileCountX, mTileCountY;
		std::unordered_map<GridPosition, AssetHandle> mSubTextures;

		friend class SubTexture2D;
		friend class TextureSheetSerialiser;
	};

	class SubTexture2D : public IRenderable, public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::SubTexture)

	public:
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2& coords, const glm::vec2& spriteSize);
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2 coords[4]);
		~SubTexture2D();

		u32 getTextureID() const override { return mSheet->mTexture->getTextureID(); }
		void bindTexture(u32 slot = 0) const override { return mSheet->mTexture->bindTexture(); }
		const glm::vec2* getTextureCoords() const override { return mTexCoords; }
		GridPosition getPosition() const;

		Ref<Texture2DSheet> getSheet() const { return mSheet; }
		Ref<Texture2D> getBaseTex() const { return mSheet->mTexture; }

	private:
		Ref<Texture2DSheet> mSheet;
		glm::vec2 mTexCoords[4];
	};

}