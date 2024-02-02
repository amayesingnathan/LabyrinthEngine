#pragma once

#include "Texture.h"

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/Filesystem.h>

namespace Laby {

	class SubTexture2D;

	//Texture2DSheet is a thin wrapper around a Texture2D for a sprite sheet which contains meta data about the sheet.
	class Texture2DSheet : public IRenderable, public Asset
	{
	public:
		ASSET_METADATA(AssetType::TextureSheet, "spritesheets")

	public:
		Texture2DSheet(const std::string& name, const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize);
		Texture2DSheet(const std::string& name, const fs::path& filepath, const glm::vec2& tileSize);

		u32 GetTextureID() const override { return mTexture->GetTextureID(); }

		std::string_view GetName() const { return mName; }
		const Ref<Texture2D>& GetBaseTex() const { return mTexture; }

		i32 GetWidth() const { return mTexture->GetWidth(); }
		i32 GetHeight() const { return mTexture->GetHeight(); }

		u32 GetTileCountX() const { return mTileCountX; }
		u32 GetTileCountY() const { return mTileCountY; }

		const glm::vec2& GetTileSize() const { return mTileSize; }

		void GenerateTileset();
		void DestroyTileset();

		AssetHandle GetFromPosition(const GridPosition& pos) const { return mSubTextures[GetPositionIndex(pos)]; }
		constexpr u32 GetPositionIndex(const GridPosition& pos) const { return (pos.y * mTileCountX) + pos.x; }

		u32 SubTexCount() const { return (u32)mSubTextures.size(); }
		const std::vector<AssetHandle>& GetSubTextures() const { return mSubTextures; }

		AssetHandle CreateSubTex(usize index, const GridPosition& coords, const glm::vec2& spriteSize = glm::vec2(1.f));
		void DestroySubTex(AssetHandle subTexHandle);

	private:
		std::string mName;
		Ref<Texture2D> mTexture;
		glm::vec2 mTileSize;
		u32 mTileCountX, mTileCountY;
		std::vector<AssetHandle> mSubTextures;

		friend class SubTexture2D;
		friend class TextureSheetSerialiser;
	};

	class SubTexture2D : public IRenderable, public Asset
	{
	public:
		ASSET_METADATA(AssetType::SubTexture, "subtextures")

	public:
		SubTexture2D(Ref<Texture2DSheet> sheet, const GridPosition& pos, const glm::vec2& spriteSize);
		SubTexture2D(Ref<Texture2DSheet> sheet, const GridPosition& pos, const glm::vec2 coords[4]);
		~SubTexture2D() = default;

		u32 GetTextureID() const override { return mSheet->mTexture->GetTextureID(); }
		const glm::vec2* GetTextureCoords() const override { return mTexCoords; }

		std::string_view GetName() const { return mName; }
		Ref<Texture2DSheet> GetSheet() const { return mSheet; }
		Ref<Texture2D> GetBaseTex() const { return mSheet->mTexture; }

	private:
		std::string mName;
		Ref<Texture2DSheet> mSheet;
		glm::vec2 mTexCoords[4];
	};

}