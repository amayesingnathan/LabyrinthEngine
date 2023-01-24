#pragma once

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Renderer/SubTexture.h>

#include "TileRenderData.h"

namespace Laby {

	struct SheetData
	{
		Ref<Texture2DSheet> sheet;
		TileID startIndex;
	};

	class TilePalette
	{
	public:
		TilePalette() = default;
		TilePalette(TileID nextIndex);

		usize getNextIndex() const { return mNextIndex; }
		const std::vector<SheetData>& getSheets() const { return mSpriteSheets; }
		const std::unordered_map<TileID, Ref<SubTexture2D>>& getTileset() const { return mTileset; }

		void add(AssetHandle spriteSheet);
		void remove(AssetHandle spriteSheet, std::unordered_map<TileID, TileID> mapping);

		Ref<SubTexture2D> operator[](TileID textureID) const;

	private:
		void MapIDs(std::unordered_map<TileID, TileID>& map, const std::unordered_map<TileID, Ref<SubTexture2D>>& oldTileset);

	private:
		std::vector<SheetData> mSpriteSheets;
		std::unordered_map<TileID, Ref<SubTexture2D>> mTileset;
		TileID mNextIndex = 0;
	};


	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TilePalette& palette)
	{
		mOut << YAML::Key << "TilePalette";
		mOut << YAML::BeginMap; // TilePalette

		LAB_SERIALISE_PROPERTY(NextIndex, palette.getNextIndex(), mOut);

		mOut << YAML::Key << "SpriteSheets";
		mOut << YAML::Value << YAML::BeginSeq;

		for (const auto& [handle, startIndex] : palette.getSheets())
		{
			mOut << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(Handle, handle, mOut);
			LAB_SERIALISE_PROPERTY(StartIndex, startIndex, mOut);
			mOut << YAML::EndMap;
		}
		mOut << YAML::EndSeq; // SpriteSheets

		mOut << YAML::Key << "Sprites";
		mOut << YAML::Value << YAML::BeginSeq;

		for (const auto& [index, sprite] : palette.getTileset())
		{
			mOut << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(Index, index, mOut);
			LAB_SERIALISE_PROPERTY_ASSET(Sprite, sprite, mOut);
			mOut << YAML::EndMap;
		}
		mOut << YAML::EndSeq; // Sprites

		mOut << YAML::EndMap; // TilePalette
	}
}

namespace YAML {

	template<>
	struct convert<Laby::TilePalette>
	{
		inline static bool decode(const Node& node, Laby::TilePalette& rhs)
		{
			auto tilePalette = node["TilePalette"];
			if (!tilePalette)
				return false;

			Laby::usize nextIndex;
			LAB_DESERIALISE_PROPERTY(NextIndex, nextIndex, tilePalette);

			auto sprites = tilePalette["Sprites"];
			if (!sprites)
				return false;

			rhs = Laby::TilePalette(nextIndex);

			for (auto sprite : sprites)
			{
				Laby::usize index;
				LAB_DESERIALISE_PROPERTY(Index, index, sprite);
				LAB_DESERIALISE_PROPERTY_ASSET(Sprite, rhs[index], sprite, SubTexture2D);
			}

			return true;
		}
	};
}