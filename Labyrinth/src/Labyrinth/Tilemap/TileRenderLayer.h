#pragma once

#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/YAML.h>

#include "TileRenderData.h"

namespace Laby {

	using TileRenderLayer = Grid<TileRenderData>;

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileRenderData& data)
	{
		mOut << YAML::BeginMap; // TileData
		LAB_SERIALISE_PROPERTY(TextureID, data.textureID, mOut);
		LAB_SERIALISE_PROPERTY(Rotation, data.rotation, mOut);
		mOut << YAML::EndMap; // TileData

		return mOut;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileRenderLayer& layer)
	{
		mOut << YAML::BeginMap; // TileRenderLayer

		LAB_SERIALISE_PROPERTY(Width, layer.GetWidth(), mOut);
		LAB_SERIALISE_PROPERTY(Height, layer.GetHeight(), mOut);

		mOut << YAML::Key << "Tiles";
		mOut << YAML::Value << YAML::BeginSeq;

		for (const auto& tileData : layer)
			mOut << tileData;

		mOut << YAML::EndSeq;

		mOut << YAML::EndMap; // TileRenderLayer

		return mOut;
	}
}

namespace YAML {

	template<>
	struct convert<Laby::TileRenderData>
	{
		inline static bool decode(const Node& node, Laby::TileRenderData& rhs)
		{
			LAB_DESERIALISE_PROPERTY(TextureID, rhs.textureID, node);
			LAB_DESERIALISE_PROPERTY(Rotation, rhs.rotation, node);

			return true;
		}
	};

	template<>
	struct convert<Laby::TileRenderLayer>
	{
		inline static bool decode(const Node& node, Laby::TileRenderLayer& rhs)
		{
			Laby::u32 width, height;
			LAB_DESERIALISE_PROPERTY(Width, width, node);
			LAB_DESERIALISE_PROPERTY(Height, height, node);

			auto tiles = node["Tiles"];
			if (!tiles)
				return false;

			rhs = Laby::TileRenderLayer(width, height);

			Laby::usize index = 0;
			for (auto tile : tiles)
				rhs.Set(index++, tile.as<Laby::TileRenderData>());

			return true;
		}
	};
}