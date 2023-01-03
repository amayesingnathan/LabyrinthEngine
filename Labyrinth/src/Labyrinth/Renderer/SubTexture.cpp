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

	bool Texture2DSheet::hasSubTex(AssetHandle id) const
	{
		return mSubTextures.contains(id);
	}

	AssetHandle Texture2DSheet::getSubTex(AssetHandle id)
	{
		if (!hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} does not exist!", id);
			return 0;
		}

		return *mSubTextures.find(id);
	}

	void Texture2DSheet::destroyTileset()
	{
		for (AssetHandle handle : mSubTextures)
			AssetManager::DestroyAsset(handle);
		FileUtils::RemoveDir(AssetManager::GetMetadata(handle).filepath.parent_path() / "subtextures");
		mSubTextures.clear();
	}

	AssetHandle Texture2DSheet::CreateSubTex(usize index, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		Ref<SubTexture2D> subTex = AssetManager::CreateNewAsset<SubTexture2D>(fmt::format("{}.lstex", index), fmt::format("spritesheets/{}/subtextures", mName), Ref<Texture2DSheet>(this), coords, spriteSize);
		return subTex->handle;
	}

	AssetHandle Texture2DSheet::CreateSubTex(usize index, const glm::vec2 coords[4])
	{
		Ref<SubTexture2D> subTex = AssetManager::CreateNewAsset<SubTexture2D>(fmt::format("{}.lstex", index), fmt::format("spritesheets/{}/subtextures", mName), Ref<Texture2DSheet>(this), coords);
		return subTex->handle;
	}

	void Texture2DSheet::GenerateTileset()
	{
		usize count = 0;
		for (u32 y = 0; y < mTileCountY; y++)
		{
			for (u32 x = 0; x < mTileCountX; x++)
				CreateSubTex(count++, { StaticCast<f32>(x), StaticCast<f32>(y) });
		}
		AssetImporter::Serialise(Ref<Texture2DSheet>(this));
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

		mSheet->mSubTextures.emplace(handle);
	}

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2 coords[4])
		: mSheet(sheet)
	{
		for (usize i = 0; i < 4; i++)
			mTexCoords[i] = coords[i];

		mSheet->mSubTextures.emplace(handle);
	}

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& tex)
	{
		//Y coordinates are flipped compared to usual so that sub texture coordinates are given from top left of sheet.
		mTexCoords[0] = { 0.f, 0.f };
		mTexCoords[1] = { 1.f, 0.f };
		mTexCoords[2] = { 1.f, 1.f };
		mTexCoords[3] = { 0.f, 1.f };

		mSheet = Ref<Texture2DSheet>::Create("NoSheetName", tex, glm::vec2{1.f, 1.f});

		mSheet->mSubTextures.emplace(handle);
	}

	SubTexture2D::~SubTexture2D()
	{
		mSheet->mSubTextures.erase(handle);
	}
}