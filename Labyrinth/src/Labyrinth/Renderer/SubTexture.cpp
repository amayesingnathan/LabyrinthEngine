#include "Lpch.h"
#include "SubTexture.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Project/Project.h>

namespace Labyrinth {

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
		if (mSubTextures.count(id) == 0)
			return false;
		else return true;
	}

	Ref<SubTexture2D> Texture2DSheet::getSubTex(i32 id)
	{
		if (!hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} does not exist!", id);
			return nullptr;
		}

		return AssetManager::GetAsset<SubTexture2D>(mSubTextures.at(id));
	}

	void Texture2DSheet::addSubTex(i32 id, const Ref<SubTexture2D>& subtex)
	{
		if (hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", id);
			return;
		}

		mSubTextures.emplace(id, subtex->handle);
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

	Ref<SubTexture2D> Texture2DSheet::createSubTex(i32 id, const std::string& name, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		if (hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", id);
			return nullptr;
		}

		Ref<SubTexture2D> subTex = AssetManager::CreateNewAsset<SubTexture2D>(name + ".lstex", "spritesheets/" + mName + "/subtextures", Ref<Texture2DSheet>(this), coords, spriteSize, name);
		mSubTextures.emplace(id, subTex->handle);
		return subTex;
	}

	Ref<SubTexture2D> Texture2DSheet::createSubTex(i32 id, const std::string& name, const glm::vec2 coords[4])
	{
		if (hasSubTex(id))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", id);
			return nullptr;
		}

		Ref<SubTexture2D> subTex = AssetManager::CreateNewAsset<SubTexture2D>(name + ".lstex", "spritesheets/" + mName + "/subtextures", Ref<Texture2DSheet>(this), coords, name);
		mSubTextures.emplace(id, subTex->handle);
		return subTex;
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
				if (mSubTextures.count(count) == 0) createSubTex(count, std::to_string(count), { Cast<f32>(x), Cast<f32>(y) });
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

	Ref<SubTexture2D> Texture2DSheet::operator[](i32 id)
	{
		if (mSubTextures.count(id) == 0) return nullptr;
		return AssetManager::GetAsset<SubTexture2D>(mSubTextures[id]);
	}

	const Ref<SubTexture2D> Texture2DSheet::operator[](i32 id) const
	{
		if (mSubTextures.count(id) == 0) return nullptr;

		return AssetManager::GetAsset<SubTexture2D>(mSubTextures.at(id));
	}

	Ref<Texture2DSheet> Texture2DSheet::Create(const std::string& filepath, const glm::vec2& tileSize, const std::string& name)
	{
		return Ref<Texture2DSheet>::Create(filepath, tileSize, name);
	}

	Ref<Texture2DSheet> Texture2DSheet::Create(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name)
	{
		return Ref<Texture2DSheet>::Create(spriteSheet, tileSize, name);
	}


	/*
		SubTexture2D
	*/

	SubTexture2D::SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2& coords, const glm::vec2& spriteSize, const std::string& name)
		: mSheet(sheet), mName(name)
	{
		f32 sheetWidth = Cast<f32>(sheet->getWidth());
		f32 sheetHeight = Cast<f32>(sheet->getHeight());

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

	SubTexture2D::SubTexture2D(const Ref<Texture2D> tex, const std::string& name)
		: mName(name)
	{
		//Y coordinates are flipped compared to usual so that sub texture coordinates are given from top left of sheet.
		mTexCoords[0] = { 0.f, 0.f };
		mTexCoords[1] = { 1.f, 0.f };
		mTexCoords[2] = { 1.f, 1.f };
		mTexCoords[3] = { 0.f, 1.f };

		mSheet = Texture2DSheet::Create(tex, { 1.f, 1.f }, name);
	}
	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2DSheet>& tex, const glm::vec2& coords, const glm::vec2& spriteSize, const std::string& name)
	{
		return Ref<SubTexture2D>::Create(tex, coords, spriteSize, name);
	}

	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2DSheet>& tex, const glm::vec2 coords[4], const std::string& name)
	{
		return Ref<SubTexture2D>::Create(tex, coords, name);
	}

	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2D>& tex, const std::string& name)
	{
		return Ref<SubTexture2D>::Create(tex, name);
	}
}