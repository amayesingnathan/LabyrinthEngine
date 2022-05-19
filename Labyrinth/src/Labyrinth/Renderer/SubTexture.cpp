#include "Lpch.h"
#include "SubTexture.h"

namespace Labyrinth {

	/*
		Texture2DSheet
	*/

	Texture2DSheet::Texture2DSheet(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name)
		: mTexture(spriteSheet), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (uint32_t)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (uint32_t)(round(mTexture->getHeight() / tileSize.y));
		mSubTextures.reserve(mTileCountX * mTileCountY);
	}

	Texture2DSheet::Texture2DSheet(const std::string& filepath, const glm::vec2& tileSize, const std::string& name)
		: mTexture(Texture2D::Create(filepath)), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (uint32_t)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (uint32_t)(round(mTexture->getHeight() / tileSize.y));
		mSubTextures.reserve(mTileCountX * mTileCountY);
	}

	bool Texture2DSheet::hasSubTex(const std::string& name) const
	{
		if (mSubTextures.find(name) == mSubTextures.end())
			return false;
		else return true;
	}

	Ref<SubTexture2D> Texture2DSheet::getSubTex(const std::string& name)
	{
		if (!hasSubTex(name))
		{
			LAB_CORE_WARN("Subtexture with the name {0} does not exist!", name);
			return nullptr;
		}

		return mSubTextures.at(name);
	}

	void Texture2DSheet::addSubTex(const std::string& name, const Ref<SubTexture2D>& subtex)
	{
		if (hasSubTex(name))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", name);
			return;
		}

		mSubTextures.emplace(name, subtex);
	}

	Ref<SubTexture2D> Texture2DSheet::createSubTex(const std::string& name, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		if (hasSubTex(name))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", name);
			return nullptr;
		}

		Ref<SubTexture2D> subTex = SubTexture2D::CreateFromCoords(CreateRefFromThis(this), coords, spriteSize, name);
		mSubTextures.emplace(name, subTex);
		return subTex;
	}

	Ref<SubTexture2D> Texture2DSheet::createSubTex(const std::string& name, const glm::vec2 coords[4])
	{
		if (hasSubTex(name))
		{
			LAB_CORE_WARN("Subtexture with the name {0} already exists!", name);
			return nullptr;
		}

		Ref<SubTexture2D> subTex = SubTexture2D::CreateFromCoords(CreateRefFromThis(this), coords, name);
		mSubTextures.emplace(name, subTex);
		return subTex;
	}

	void Texture2DSheet::deleteSubTex(const std::string& name)
	{
		if (!hasSubTex(name))
		{
			LAB_CORE_WARN("Subtexture with the name {0} does not exist!", name);
			return;
		}

		mSubTextures.erase(name);
	}

	Ref<Texture2DSheet> Texture2DSheet::CreateFromPath(const std::string& filepath, const glm::vec2& tileSize, const std::string& name)
	{
		return CreateRef<Texture2DSheet>(filepath, tileSize, name);
	}

	Ref<Texture2DSheet> Texture2DSheet::CreateFromTex(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name)
	{
		return CreateRef<Texture2DSheet>(spriteSheet, tileSize, name);
	}


	/*
		SubTexture2D
	*/

	SubTexture2D::SubTexture2D(const Ref<Texture2DSheet> sheet, const glm::vec2& min, const glm::vec2& max, const std::string& name)
		: mSheet(sheet), mName(name)
	{
		//Y coordinates are flipped compared to usual so that sub texture coordinates are given from top left of sheet.
		mTexCoords[0] = { min.x , max.y };
		mTexCoords[1] = { max.x , max.y };
		mTexCoords[2] = { max.x , min.y };
		mTexCoords[3] = { min.x , min.y };
	}

	SubTexture2D::SubTexture2D(const Ref<Texture2DSheet> sheet, const glm::vec2 coords[4], const std::string& name)
		: mSheet(sheet), mName(name)
	{
		for (size_t i = 0; i < 4; i++)
			mTexCoords[i] = coords[i];
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2DSheet>& tex, const glm::vec2& coords, const glm::vec2& spriteSize, const std::string& name)
	{
		const uint32_t sheetWidth = tex->getWidth();
		const uint32_t sheetHeight = tex->getHeight();

		//Calculate y coord as Sheet Height - y so that "coords" parameter can be given as position from top right
		glm::vec2 min = { {(coords.x * tex->mTileSize.x) / sheetWidth}, {(sheetHeight - (coords.y * tex->mTileSize.y)) / sheetHeight} };
		glm::vec2 max = { {((coords.x + spriteSize.x) * tex->mTileSize.x) / sheetWidth}, {(sheetHeight - ((coords.y + spriteSize.y) * tex->mTileSize.y)) / sheetHeight} };
		return CreateRef<SubTexture2D>(tex, min, max, name);
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2DSheet>& tex, const glm::vec2 coords[4], const std::string& name)
	{
		const uint32_t sheetWidth = tex->getWidth();
		const uint32_t sheetHeight = tex->getHeight();

		//Calculate y coord as Sheet Height - y so that "coords" parameter can be given as position from top right
		return CreateRef<SubTexture2D>(tex, coords, name);
	}

}