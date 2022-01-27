#include "Lpch.h"
#include "SubTexture.h"

namespace Labyrinth {

	/*
		Texture2DSheet
	*/

	Texture2DSheet::Texture2DSheet(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize)
		: mTexture(spriteSheet), mTileSize(tileSize)
	{
		mTileCountX = (uint32_t)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (uint32_t)(round(mTexture->getHeight() / tileSize.y));
		mSubTextures.reserve(mTileCountX * mTileCountY);
	}

	Texture2DSheet::Texture2DSheet(const std::string& filepath, const glm::vec2& tileSize)
		: mTexture(Texture2D::Create(filepath)), mTileSize(tileSize)
	{
		mTileCountX = (uint32_t)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (uint32_t)(round(mTexture->getHeight() / tileSize.y));
		mSubTextures.reserve(mTileCountX * mTileCountY);
	}

	Ref<SubTexture2D> Texture2DSheet::getSubTex(const std::string& name) const
	{
		if (mSubTextures.find(name) == mSubTextures.end())
		{
			LAB_CORE_WARN("Subtexture with the name {0} does not exist!", name);
			return nullptr;
		}

		return mSubTextures.at(name);
	}

	void Texture2DSheet::addSubTex(const std::string& name, const Ref<SubTexture2D>& subtex)
	{
		if (mSubTextures.find(name) != mSubTextures.end())
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", name);
			return;
		}

		mSubTextures.emplace(name, subtex);
	}

	Ref<SubTexture2D> Texture2DSheet::createSubTex(const std::string& name, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		if (mSubTextures.find(name) != mSubTextures.end())
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", name);
			return nullptr;
		}

		Ref<SubTexture2D> subTex = SubTexture2D::CreateFromCoords(CloneRef<Texture2DSheet>(*this), coords, spriteSize);
		mSubTextures.emplace(name, subTex);
		return subTex;
	}

	void Texture2DSheet::deleteSubTex(const std::string& name)
	{
		if (mSubTextures.find(name) == mSubTextures.end())
		{
			LAB_CORE_WARN("Subtexture with the name {0} does not exist!", name);
			return;
		}

		mSubTextures.erase(name);
	}

	Ref<Texture2DSheet> Texture2DSheet::CreateFromPath(const std::string& filepath, const glm::vec2& tileSize)
	{
		return CreateRef<Texture2DSheet>(filepath, tileSize);
	}

	Ref<Texture2DSheet> Texture2DSheet::CreateFromTex(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize)
	{
		return CreateRef<Texture2DSheet>(spriteSheet, tileSize);
	}


	/*
		SubTexture2D
	*/

	SubTexture2D::SubTexture2D(const Ref<Texture2DSheet> sheet, const glm::vec2& min, const glm::vec2& max)
		: mSheet(sheet)
	{
		//Y coordinates are flipped compared to usual so that sub texture coordinates are given from top left of sheet.
		mTexCoords[0] = { min.x , max.y };
		mTexCoords[1] = { max.x , max.y };
		mTexCoords[2] = { max.x , min.y };
		mTexCoords[3] = { min.x , min.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2DSheet>& tex, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		const uint32_t sheetWidth = tex->getWidth();
		const uint32_t sheetHeight = tex->getHeight();

		//Calculate y coord as Sheet Height - y so that "coords" parameter can be given as position from top right
		glm::vec2 min = { {(coords.x * tex->mTileSize.x) / sheetWidth}, {(sheetHeight - (coords.y * tex->mTileSize.y)) / sheetHeight} };
		glm::vec2 max = { {((coords.x + spriteSize.x) * tex->mTileSize.x) / sheetWidth}, {(sheetHeight - ((coords.y + spriteSize.y) * tex->mTileSize.y)) / sheetHeight} };
		return CreateRef<SubTexture2D>(tex, min, max);
	}

}