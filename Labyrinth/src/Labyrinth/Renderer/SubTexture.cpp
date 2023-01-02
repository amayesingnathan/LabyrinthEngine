#include "Lpch.h"
#include "SubTexture.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Project/Project.h>

namespace Laby {

	/*
		Texture2DSheet
	*/

	Texture2DSheet::Texture2DSheet(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name)
		: mTexture(spriteSheet), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (u32)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (u32)(round(mTexture->getHeight() / tileSize.y));
	}

	Texture2DSheet::Texture2DSheet(const std::string& filepath, const glm::vec2& tileSize, const std::string& name)
		: mTexture(AssetManager::GetAsset<Texture2D>(filepath)), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (u32)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (u32)(round(mTexture->getHeight() / tileSize.y));
	}

	bool Texture2DSheet::hasSubTex(i32 id) const
	{
		if (mSubTextures.contains(id))
			return false;
		else return true;
	}

	AssetHandle Texture2DSheet::getSubTex(i32 id)
	{
		if (!hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} does not exist!", id);
			return 0;
		}

		return mSubTextures.at(id);
	}

	void Texture2DSheet::addSubTex(i32 id, AssetHandle handle)
	{
		if (hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", id);
			return;
		}

		mSubTextures.emplace(id, handle);
	}

	AssetHandle Texture2DSheet::createSubTex(i32 id, const std::string& name, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		if (hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", id);
			return 0;
		}

		Ref<SubTexture2D> subTex = AssetManager::CreateNewAsset<SubTexture2D>(name + ".lstex", "spritesheets/" + mName + "/subtextures", Ref<Texture2DSheet>(this), coords, spriteSize, name);
		mSubTextures.emplace(id, subTex->handle);
		return subTex->handle;
	}

	AssetHandle Texture2DSheet::createSubTex(i32 id, const std::string& name, const glm::vec2 coords[4])
	{
		if (hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", id);
			return 0;
		}

		Ref<SubTexture2D> subTex = AssetManager::CreateNewAsset<SubTexture2D>(name + ".lstex", "spritesheets/" + mName + "/subtextures", Ref<Texture2DSheet>(this), coords, name);
		mSubTextures.emplace(id, subTex->handle);
		return subTex->handle;
	}

	void Texture2DSheet::deleteSubTex(i32 id)
	{
		if (!hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the id {0} does not exist!", id);
			return;
		}

		mSubTextures.erase(id);
	}

	void Texture2DSheet::generateTileset(i32 startIndex)
	{
		i32 count = startIndex;
		for (u32 y = 0; y < mTileCountY; y++)
		{
			for (u32 x = 0; x < mTileCountX; x++)
			{
				if (mSubTextures.count(count) == 0) createSubTex(count, std::to_string(count), { StaticCast<f32>(x), StaticCast<f32>(y) });
				count++;
			}
		}
		AssetImporter::Serialise(Ref<Texture2DSheet>(this));
	}

	void Texture2DSheet::clearTileset()
	{
		for (const auto& [id, handle] : mSubTextures)
			AssetManager::DestroyAsset(handle);
		FileUtils::RemoveDir(AssetManager::GetMetadata(handle).filepath.parent_path() / "subtextures");

		mSubTextures.clear();
	}

	AssetHandle Texture2DSheet::operator[](i32 id) const
	{
		if (mSubTextures.contains(id)) 
			return 0;

		return mSubTextures.at(id);
	}

	/*
		SubTexture2D
	*/

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2& coords, const glm::vec2& spriteSize, const std::string& name)
		: mSheet(sheet), mName(name)
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

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2 coords[4], const std::string& name)
		: mSheet(sheet), mName(name)
	{
		for (usize i = 0; i < 4; i++)
			mTexCoords[i] = coords[i];
	}

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& tex, const std::string& name)
		: mName(name)
	{
		//Y coordinates are flipped compared to usual so that sub texture coordinates are given from top left of sheet.
		mTexCoords[0] = { 0.f, 0.f };
		mTexCoords[1] = { 1.f, 0.f };
		mTexCoords[2] = { 1.f, 1.f };
		mTexCoords[3] = { 0.f, 1.f };

		mSheet = Ref<Texture2DSheet>::Create(tex, glm::vec2{ 1.f, 1.f }, name);
	}
}