#pragma once

#include "Texture.h"

#include <Labyrinth/Core/System/Cast.h>

#include <glm/glm.hpp>

#include <unordered_map>

namespace Labyrinth {

	class SubTexture2D;

	//Texture2DSheet is a thin wrapper around a Texture2D for a sprite sheet which contains meta data about the sheet.
	class Texture2DSheet : public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::TextureSheet)

	public:
		Texture2DSheet(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name);
		Texture2DSheet(const std::string& filepath, const glm::vec2& tileSize, const std::string& name);
		Texture2DSheet(const Texture2DSheet&) = default;
		virtual ~Texture2DSheet() = default;

		operator Ref<Texture2D>() const { return mTexture; }

		usize getWidth() const { return mTexture->getWidth(); }
		usize getHeight() const { return mTexture->getHeight(); }

		u32 getTileCountX() const { return mTileCountX; }
		u32 getTileCountY() const { return mTileCountY; }
		
		glm::vec2 getTileSize() const { return mTileSize; }
		u32 getTileWidth() const { return Cast<u32>(mTileSize.x); }
		u32 getTileHeight() const { return Cast<u32>(mTileSize.y); }

		glm::vec2 getTileSizeN() const { return { mTileSize.x / mTexture->getWidth(), mTileSize.y / mTexture->getHeight() }; }

		const std::string& getName() const { return mName; }
		const Ref<Texture2D>& getBaseTex() const { return mTexture; }

		bool hasSubTex(const std::string& name) const;
		Ref<SubTexture2D> getSubTex(const std::string& name);

		std::unordered_map<std::string, AssetHandle>& getSubTexList() { return mSubTextures; }
		const std::unordered_map<std::string, AssetHandle>& getSubTexList() const { return mSubTextures; }

		void addSubTex(const std::string& name, const Ref<SubTexture2D>& subtex);
		void addSubTex(const std::string& name, AssetHandle handle);
		Ref<SubTexture2D> createSubTex(const std::string& name, const glm::vec2& coords, const glm::vec2& spriteSize = glm::vec2(1.f));
		Ref<SubTexture2D> createSubTex(const std::string& name, const glm::vec2 coords[4]);
		void deleteSubTex(const std::string& name);

		void generateTileset(usize startIndex = 0);

		Ref<SubTexture2D> operator[] (const std::string& key);
		const Ref<SubTexture2D> operator[] (const std::string& key) const;

		static Ref<Texture2DSheet> Create(const std::string& filepath, const glm::vec2& tileSize, const std::string& name = "");
		static Ref<Texture2DSheet> Create(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize, const std::string& name = "");

	private:
		std::string mName;
		Ref<Texture2D> mTexture;
		glm::vec2 mTileSize;
		u32 mTileCountX, mTileCountY;
		std::unordered_map<std::string, AssetHandle> mSubTextures;
		
		friend class SubTexture2D;
	};

	class SubTexture2D : public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::SubTexture)

	public:
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2& coords, const glm::vec2& spriteSize, const std::string& name);
		SubTexture2D(Ref<Texture2DSheet> sheet, const glm::vec2 coords[4], const std::string& name);
		SubTexture2D(const Ref<Texture2D> sheet, const std::string& name = "");
		SubTexture2D(const SubTexture2D&) = default;
		virtual ~SubTexture2D() = default;

		Ref<Texture2DSheet> getSheet() const { return mSheet; }
		Ref<Texture2D> getBaseTex() const { return mSheet->mTexture; }

		const std::string& getName() const { return mName; }
		glm::vec2* getTexCoords() { return mTexCoords; }
		const glm::vec2* getTexCoords() const { return mTexCoords; }

		static Ref<SubTexture2D> Create(const Ref<Texture2DSheet>& tex, const glm::vec2& coords, const glm::vec2& spriteSize = glm::vec2(1.f), const std::string& name = "");
		static Ref<SubTexture2D> Create(const Ref<Texture2DSheet>& tex, const glm::vec2 coords[4], const std::string& name = "");
		static Ref<SubTexture2D> Create(const Ref<Texture2D>& tex, const std::string& name = "");

	private:
		std::string mName;
		Ref<Texture2DSheet> mSheet;
		glm::vec2 mTexCoords[4];

	};

}