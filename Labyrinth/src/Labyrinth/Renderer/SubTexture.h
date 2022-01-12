#pragma once

#include <glm/glm.hpp>

#include "Texture.h"
#include "Labyrinth/Maths/Quad.h"

namespace Labyrinth {

	class SubTexture2D;

	//Texture2DSheet is a thin wrapper around a Texture2D for sprite sheets which contains meta data about it,
	//as well as an API for retrieving tiles from the sheet.
	class Texture2DSheet
	{
	public:
		Texture2DSheet(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize);
		Texture2DSheet(const std::string& filepath, const glm::vec2& tileSize);
		~Texture2DSheet();

		operator Ref<Texture2D>() const { return mTexture; }

		uint32_t getWidth() const { return mTexture->getWidth(); }
		uint32_t getHeight() const { return mTexture->getHeight(); }

		Ref<Texture2D> getTex() const { return mTexture; }
		Ref<SubTexture2D> getSubTex(const std::string& name) const;

		Ref<SubTexture2D> createSubTex(const std::string& name, const glm::vec2& coords, const glm::vec2& spriteSize = glm::vec2{ 1.0f });

		static Ref<Texture2DSheet> CreateFromPath(const std::string& filepath, const glm::vec2& tileSize);
		static Ref<Texture2DSheet> CreateFromTex(const Ref<Texture2D>& spriteSheet, const glm::vec2& tileSize);

	private:
		Ref<Texture2D> mTexture;
		glm::vec2 mTileSize;
		std::unordered_map <std::string, Ref<SubTexture2D>> mSubTextures;
		
		friend SubTexture2D;
	};

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2DSheet> sheet, const glm::vec2& min, const glm::vec2& max);
		~SubTexture2D() = default;

		const Ref<Texture2DSheet>& getSheet() { return mSheet; }
		const Ref<Texture2D>& getTex() { return mSheet->mTexture; }
		const glm::vec2* getTexCoords() { return mTexCoords; }

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2DSheet>& tex, const glm::vec2& coords, const glm::vec2& spriteSize = glm::vec2{ 1.0f });

	private:
		Ref<Texture2DSheet> mSheet;
		glm::vec2 mTexCoords[4];

	};

}