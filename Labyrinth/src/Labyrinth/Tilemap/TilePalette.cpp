#include "Lpch.h"
#include "TilePalette.h"

namespace Laby {

	static bool Contains(AssetHandle handle, const std::vector<SheetData>& sheets)
	{
		return std::find_if(sheets.begin(), sheets.end(), [&](const SheetData& data) { return data.sheet->handle == handle; }) != sheets.end();
	}

	Ref<SubTexture2D> TilePalette::operator[](TileID textureID) const
	{
		if (textureID == NullTileID || !mTileset.contains(textureID))
			return nullptr;

		return mTileset.at(textureID);
	}

	void TilePalette::add(AssetHandle sheetHandle)
	{
		if (Contains(sheetHandle, mSpriteSheets))
			return;

		bool hasSheet = mSpriteSheets.empty();
		const SheetData& last = hasSheet ? mSpriteSheets.back() : SheetData();
		usize sheetTexCount = hasSheet ? last.sheet->subTexCount() : 0;

		Ref<Texture2DSheet> spriteSheet = AssetManager::GetAsset<Texture2DSheet>(sheetHandle);
		TileID nextIndex = last.startIndex + sheetTexCount;
		mSpriteSheets.emplace_back(spriteSheet, nextIndex);

		for (AssetHandle subtex : spriteSheet->getSubTextures())
			mTileset[nextIndex++] = AssetManager::GetAsset<SubTexture2D>(subtex);
	}

	void TilePalette::add(AssetHandle sheetHandle, TileID nextIndex)
	{
		if (Contains(sheetHandle, mSpriteSheets))
			return;

		Ref<Texture2DSheet> spriteSheet = AssetManager::GetAsset<Texture2DSheet>(sheetHandle);
		mSpriteSheets.emplace_back(spriteSheet, nextIndex);

		for (AssetHandle subtex : spriteSheet->getSubTextures())
			mTileset[nextIndex++] = AssetManager::GetAsset<SubTexture2D>(subtex);
	}

	void TilePalette::remove(AssetHandle sheetHandle, std::unordered_map<TileID, TileID>& mapping)
	{
		if (!Contains(sheetHandle, mSpriteSheets))
			return;

		auto tileset(std::move(mTileset));
		std::erase_if(mSpriteSheets, [&](const SheetData& data) { return data.sheet->handle == sheetHandle; });

		TileID lastIndex = 0;
		usize lastSize = 0;

		for (SheetData& data : mSpriteSheets)
		{
			data.startIndex = lastIndex + lastSize;

			lastIndex = data.startIndex;
			lastSize = data.sheet->subTexCount();
		}

		mTileset.clear();
		usize nextIndex = 0;
		for (const SheetData& data : mSpriteSheets)
		{
			for (AssetHandle subtex : data.sheet->getSubTextures())
				mTileset.emplace(nextIndex++, AssetManager::GetAsset<SubTexture2D>(subtex));
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