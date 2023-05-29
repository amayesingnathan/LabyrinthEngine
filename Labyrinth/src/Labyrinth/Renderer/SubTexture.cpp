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

	Texture2DSheet::Texture2DSheet(const std::string& name, const fs::path& filepath, const glm::vec2& tileSize)
		: mTexture(AssetManager::GetAsset<Texture2D>(filepath)), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (u32)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (u32)(round(mTexture->getHeight() / tileSize.y));
	}

	void Texture2DSheet::generateTileset()
	{
		const fs::path& subtexDir = Project::GetAssetDirectory() / SubTexture2D::GetAssetDirectory() / mName;
		if (!fs::exists(subtexDir))
			FileUtils::CreateDir(subtexDir);

		mSubTextures.clear();
		mSubTextures.reserve(mTileCountX * mTileCountY);

		usize count = 0;
		for (u32 y = 0; y < mTileCountY; y++)
		{
			for (u32 x = 0; x < mTileCountX; x++)
				createSubTex(count++, { x, y });
		}
		AssetImporter::Serialise(Ref<Texture2DSheet>(this));
		AssetManager::ReloadAssets();
	}

	void Texture2DSheet::destroyTileset()
	{
		for (AssetHandle handle : mSubTextures)
			AssetManager::DestroyAsset(handle);
		FileUtils::RemoveDir(AssetManager::GetMetadata(handle).filepath.parent_path() / "subtextures");
		mSubTextures.clear();
	}

	AssetHandle Texture2DSheet::createSubTex(usize index, const GridPosition& coords, const glm::vec2& spriteSize)
	{
		AssetHandle newTex = 0;
		if (AssetManager::IsMemoryAsset(handle))
			newTex = AssetManager::CreateMemoryOnlyAsset<SubTexture2D>(Ref<Texture2DSheet>(this), coords, spriteSize);
		else
			newTex = AssetManager::CreateNewAsset<SubTexture2D>(std::format("{}/{}", mName, index), Ref<Texture2DSheet>(this), coords, spriteSize)->handle;
		mSubTextures.emplace_back(newTex);
		return newTex;
	}

	void Texture2DSheet::destroySubTex(AssetHandle subTexHandle)
	{
		auto it = std::ranges::find(mSubTextures, subTexHandle);
		if (it == mSubTextures.end())
			return;

		mSubTextures.erase(it);
		AssetManager::DestroyAsset(subTexHandle);
	}

	/*
		SubTexture2D
	*/

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const GridPosition& pos, const glm::vec2& spriteSize)
		: mName(fmt::format("{} - ({}, {})", sheet->getName(), pos.x, pos.y)), mSheet(sheet)
	{
		glm::vec2 coords{ (f32)pos.x, (f32)pos.y };
		f32 sheetWidth = (f32)sheet->getWidth();
		f32 sheetHeight = (f32)sheet->getHeight();

		glm::vec2 min = { (coords.x * sheet->mTileSize.x) / sheetWidth, (sheetHeight - (coords.y * sheet->mTileSize.y)) / sheetHeight };
		glm::vec2 max = { ((coords.x + spriteSize.x) * sheet->mTileSize.x) / sheetWidth, (sheetHeight - ((coords.y + spriteSize.y) * sheet->mTileSize.y)) / sheetHeight };

		mTexCoords[0] = { min.x, max.y };
		mTexCoords[1] = { max.x, max.y };
		mTexCoords[2] = { max.x, min.y };
		mTexCoords[3] = { min.x, min.y };
	}

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const GridPosition& pos, const glm::vec2 coords[4])
		: mName(fmt::format("{} - ({}, {})", sheet->getName(), pos.x, pos.y)), mSheet(sheet)
	{
		for (usize i = 0; i < 4; i++)
			mTexCoords[i] = coords[i];
	}
}