#pragma once

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	class TilePalette
	{
	public:
		TilePalette() = default;
		TilePalette(usize nextIndex);

		Ref<SubTexture2D>& operator[](usize textureID) { return mTileset[textureID]; }
		const Ref<SubTexture2D>& operator[](usize textureID) const;

		void add(AssetHandle spriteSheet);

		usize getNextIndex() const { return mNextIndex; }
		const std::unordered_map<usize, Ref<SubTexture2D>>& getTileset() const { return mTileset; }

	private:
		std::unordered_map<usize, Ref<SubTexture2D>> mTileset;
		usize mNextIndex = 0;
	};


	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TilePalette& palette)
	{
		mOut << YAML::Key << "TilePalette";
		mOut << YAML::BeginMap; // TilePalette

		LAB_SERIALISE_PROPERTY(NextIndex, palette.getNextIndex(), mOut);

		mOut << YAML::Key << "Sprites";
		mOut << YAML::Value << YAML::BeginSeq;

		for (const auto& [index, sprite] : palette.getTileset())
		{
			mOut << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(Index, index, mOut);
			LAB_SERIALISE_PROPERTY(Sprite, sprite->handle, mOut);
			mOut << YAML::EndMap;
		}
		mOut << YAML::EndSeq;

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