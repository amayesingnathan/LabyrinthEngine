#include "Lpch.h"
#include "TilePalette.h"

namespace Laby {

	TilePalette::TilePalette(usize nextIndex)
		: mNextIndex(nextIndex)
	{
	}

	const Ref<SubTexture2D>& TilePalette::operator[](usize textureID) const
	{
		LAB_CORE_ASSERT(mTileset.contains(textureID), "Texture ID is not in tile palette!");
		return mTileset.at(textureID);
	}

	void TilePalette::add(AssetHandle spriteSheet)
	{
		LAB_CORE_ASSERT(!mSpriteSheets.contains(spriteSheet), "Palette already has this spritesheet!");

		mSpriteSheets.emplace(spriteSheet);
		Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(spriteSheet);
		for (AssetHandle sprite : sheet->getSubTextures())
			mTileset.emplace(mNextIndex++, AssetManager::GetAsset<SubTexture2D>(sprite));
	}
}