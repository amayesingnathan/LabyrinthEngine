#include "Lpch.h"
#include "SubTexture.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Project/Project.h>

namespace Laby {

	/*
		Texture2DSheet
	*/

	Texture2DSheet::Texture2DSheet(const std::string& name, const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize)
		: mTexture(spriteSheet), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (u32)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (u32)(round(mTexture->getHeight() / tileSize.y));
	}

	Texture2DSheet::Texture2DSheet(const std::string& name, const std::string& filepath, const glm::vec2& tileSize)
		: mTexture(AssetManager::GetAsset<Texture2D>(filepath)), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (u32)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (u32)(round(mTexture->getHeight() / tileSize.y));
	}

	void Texture2DSheet::generateTileset()
	{
		usize count = 0;
		for (u32 y = 0; y < mTileCountY; y++)
		{
			for (u32 x = 0; x < mTileCountX; x++)
				CreateSubTex(count++, { x, y });
		}
		AssetImporter::Serialise(Ref<Texture2DSheet>(this));
	}

	void Texture2DSheet::destroyTileset()
	{
		for (AssetHandle handle : mSubTextures)
			AssetManager::DestroyAsset(handle);
		FileUtils::RemoveDir(AssetManager::GetMetadata(handle).filepath.parent_path() / "subtextures");
		mSubTextures.clear();
	}

	void Texture2DSheet::CreateSubTex(usize index, const GridPosition& coords, const glm::vec2& spriteSize)
	{
		glm::vec2 coordsF = { (f32)coords.x, (f32)coords.y };
		Ref<SubTexture2D> subTex = AssetManager::CreateNewAsset<SubTexture2D>(fmt::format("{}.lstex", index), 
																			  fmt::format("spritesheets/{}/subtextures", mName), 
																			  Ref<Texture2DSheet>(this), coordsF, spriteSize);
		mSubTextures.emplace_back(subTex->handle);
	}

	/*
		SubTexture2D
	*/

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2& coords, const glm::vec2& spriteSize)
		: mSheet(sheet)
	{
		f32 sheetWidth = StaticCast<f32>(sheet->getWidth());
		f32 sheetHeight = StaticCast<f32>(sheet->getHeight());

		//Calculate y coord as Sheet Height - y so that "coords" parameter can be given as position from top right
		glm::vec2 min = { ((coords.x * sheet->mTileSize.x) / sheetWidth), ((sheetHeight - (coords.y * sheet->mTileSize.y)) / sheetHeight) };
		glm::vec2 max = { (((coords.x + spriteSize.x) * sheet->mTileSize.x) / sheetWidth), ((sheetHeight - ((coords.y + spriteSize.y) * sheet->mTileSize.y)) / sheetHeight) };

		//Y coordinates are flipped compared to usual so that sub texture coordinates are given from top left of sheet.
		mTexCoords[0] = { min.x , max.y };
		mTexCoords[1] = { max.x , max.y };
		mTexCoords[2] = { max.x , min.y };
		mTexCoords[3] = { min.x , min.y };
	}

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2 coords[4])
		: mSheet(sheet)
	{
		for (usize i = 0; i < 4; i++)
			mTexCoords[i] = coords[i];
	}

	GridPosition SubTexture2D::getPosition() const
	{
		glm::vec2 pos = mTexCoords[3] / mSheet->mTileSize;
		return { (usize)std::round(pos.x), (usize)std::round(pos.y) };
	}
}