#pragma once

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Renderer/SubTexture.h>

#include "TileRenderData.h"

namespace Laby {

	struct SheetData
	{
		Ref<Texture2DSheet> sheet = nullptr;
		TileID startIndex = 0;

		SheetData() = default;
		SheetData(Ref<Texture2DSheet> spriteSheet, TileID index) : sheet(spriteSheet), startIndex(index) {}
	};

	class TilePalette
	{
	public:
		TilePalette() = default;

		const std::vector<SheetData>& GetSheets() const { return mSpriteSheets; }
		const std::unordered_map<TileID, Ref<SubTexture2D>>& GetTileset() const { return mTileset; }

		void Add(AssetHandle sheetHandle);
		void Add(AssetHandle sheetHandle, TileID nextIndex);
		void Remove(AssetHandle sheetHandle, std::unordered_map<TileID, TileID>& mapping);

		Ref<SubTexture2D> operator[](TileID textureID) const;

	private:
		void MapIDs(std::unordered_map<TileID, TileID>& map, const std::unordered_map<TileID, Ref<SubTexture2D>>& oldTileset);

	private:
		std::vector<SheetData> mSpriteSheets;
		std::unordered_map<TileID, Ref<SubTexture2D>> mTileset;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const SheetData& palette)
	{
		mOut << YAML::BeginMap;
		LAB_SERIALISE_PROPERTY_ASSET(Sheet, palette.sheet, mOut);
		LAB_SERIALISE_PROPERTY(StartIndex, palette.startIndex, mOut);
		mOut << YAML::EndMap;

		return mOut;
	}
}

namespace YAML {

	template<>
	struct convert<Laby::SheetData>
	{
		inline static bool decode(const Node& node, Laby::SheetData& rhs)
		{
			LAB_DESERIALISE_PROPERTY_ASSET(Sheet, rhs.sheet, node, Texture2DSheet);
			LAB_DESERIALISE_PROPERTY(StartIndex, rhs.startIndex, node);

			if (!rhs.sheet)
				return false;

			return true;
		}
	};
}