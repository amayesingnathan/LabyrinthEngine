#include "Lpch.h"
#include "SubTexture.h"

namespace Labyrinth {

	/*
		Texture2DSheet
	*/

	Texture2DSheet::Texture2DSheet(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name)
		: mTexture(spriteSheet), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (u32)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (u32)(round(mTexture->getHeight() / tileSize.y));
		mSubTextures.reserve(mTileCountX * mTileCountY);
	}

	Texture2DSheet::Texture2DSheet(const std::string& filepath, const glm::vec2& tileSize, const std::string& name)
		: mTexture(Texture2D::Create(filepath)), mTileSize(tileSize), mName(name)
	{
		mTileCountX = (u32)(round(mTexture->getWidth() / tileSize.x));
		mTileCountY = (u32)(round(mTexture->getHeight() / tileSize.y));
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

		Ref<SubTexture2D> subTex = SubTexture2D::Create(CreateRef(this), coords, spriteSize, name);
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

		Ref<SubTexture2D> subTex = SubTexture2D::Create(CreateRef(this), coords, name);
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

	void Texture2DSheet::generateTileset(usize startIndex)
	{
		mSubTextures.clear();

		usize count = startIndex;
		for (u32 y = 0; y < mTileCountY; y++)
		{
			for (u32 x = 0; x < mTileCountX; x++)
			{
				createSubTex(std::to_string(count), { Cast<f32>(x), Cast<f32>(y) });
				count++;
			}
		}
	}

	Ref<SubTexture2D> Texture2DSheet::operator[](const std::string& key)
	{
		if (mSubTextures.count(key) == 0) return nullptr;
		return mSubTextures[key];
	}

	const Ref<SubTexture2D> Texture2DSheet::operator[](const std::string& key) const
	{
		if (mSubTextures.count(key) == 0) return nullptr;

		return mSubTextures.at(key);
	}

	Ref<Texture2DSheet> Texture2DSheet::Create(const std::string& filepath, const glm::vec2& tileSize, const std::string& name)
	{
		return CreateRef<Texture2DSheet>(filepath, tileSize, name);
	}

	Ref<Texture2DSheet> Texture2DSheet::Create(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name)
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
		f32 sheetWidth = Cast<f32>(tex->getWidth());
		f32 sheetHeight = Cast<f32>(tex->getHeight());

		//Calculate y coord as Sheet Height - y so that "coords" parameter can be given as position from top right
		glm::vec2 min = { ((coords.x * tex->mTileSize.x) / sheetWidth), ((sheetHeight - (coords.y * tex->mTileSize.y)) / sheetHeight) };
		glm::vec2 max = { (((coords.x + spriteSize.x) * tex->mTileSize.x) / sheetWidth), ((sheetHeight - ((coords.y + spriteSize.y) * tex->mTileSize.y)) / sheetHeight) };
		return CreateRef<SubTexture2D>(tex, min, max, name);
	}

	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2DSheet>& tex, const glm::vec2 coords[4], const std::string& name)
	{
		return CreateRef<SubTexture2D>(tex, coords, name);
	}

	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2D>& tex, const std::string& name)
	{
		return CreateRef<SubTexture2D>(tex, name);
	}
}