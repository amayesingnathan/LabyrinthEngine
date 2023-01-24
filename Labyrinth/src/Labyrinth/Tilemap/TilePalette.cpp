#include "Lpch.h"
#include "TilePalette.h"

namespace Laby {

	TilePalette::TilePalette(TileID nextIndex)
		: mNextIndex(nextIndex)
	{
	}

	Ref<SubTexture2D> TilePalette::operator[](TileID textureID) const
	{
		if (textureID == NullTileID || !mTileset.contains(textureID))
			return nullptr;

		return mTileset.at(textureID);
	}

	void TilePalette::add(AssetHandle spriteSheet)
	{
		LAB_CORE_ASSERT(mSpriteSheets.find(spriteSheet) == mSpriteSheets.end(), "Palette already has this spritesheet!");

		const SheetData& last = mSpriteSheets.back();
		Ref<Texture2DSheet>

		mSpriteSheets.emplace_back(spriteSheet);
		Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(spriteSheet);
		for (AssetHandle sprite : sheet->getSubTextures())
			mTileset.emplace(mNextIndex++, AssetManager::GetAsset<SubTexture2D>(sprite));
	}

	void TilePalette::remove(AssetHandle spriteSheet, std::unordered_map<TileID, TileID> mapping)
	{
		LAB_CORE_ASSERT(mSpriteSheets.find(spriteSheet) != mSpriteSheets.end(), "Palette does not contain this spritesheet!");

		auto tileset(std::move(mTileset));

		std::erase(mSpriteSheets, spriteSheet);
		mTileset.clear();
		mNextIndex = 0;

		for (AssetHandle handle : mSpriteSheets)
		{
			Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(handle);
			for (AssetHandle sprite : sheet->getSubTextures())
				mTileset.emplace(mNextIndex++, AssetManager::GetAsset<SubTexture2D>(sprite));
		}

		MapIDs(mapping, tileset);
	}

	void TilePalette::MapIDs(std::unordered_map<TileID, TileID>& map, const std::unordered_map<TileID, Ref<SubTexture2D>>& oldTileset)
	{
		std::unordered_map<Ref<SubTexture2D>, TileID> reverseTileset;
		for (const auto& [index, tex] : mTileset)
			reverseTileset[tex] = index;

		usize size = mTileset.size();
		for (const auto& [index, tex] : oldTileset)
			map[index] = (index < size) ? reverseTileset[tex] : NullTileID;
	}
}