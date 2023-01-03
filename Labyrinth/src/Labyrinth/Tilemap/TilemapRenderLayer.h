#pragma once

#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/YAML.h>

namespace Laby {

	struct TileRenderData
	{
		usize textureID;
		f32 rotation;
	};

	class TilemapRenderLayer : public Grid<TileRenderData>
	{
	public:
		TilemapRenderLayer(usize layer, usize width, usize height)
			: Grid<TileRenderData>(width, height), mIndex(layer) {}

		bool operator==(const TilemapRenderLayer& other) const { return mIndex == other.mIndex; }

		usize getLayer() const { return mIndex; }

	private:
		usize mIndex = 0;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileRenderData& data)
	{
		mOut << YAML::Key << "TileData";
		mOut << YAML::BeginMap; // TileData

		LAB_SERIALISE_PROPERTY(TextureID, data.textureID, mOut);
		LAB_SERIALISE_PROPERTY(Rotation, data.rotation, mOut);

		mOut << YAML::EndMap; // TileData
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TilemapRenderLayer& layer)
	{
		mOut << YAML::Key << "TilemapRenderLayer";
		mOut << YAML::BeginMap; // TilemapRenderLayer

		LAB_SERIALISE_PROPERTY(Layer, layer.getLayer(), mOut);
		LAB_SERIALISE_PROPERTY(Width, layer.getWidth(), mOut);
		LAB_SERIALISE_PROPERTY(Height, layer.getHeight(), mOut);

		mOut << YAML::Key << "Tiles";
		mOut << YAML::Value << YAML::BeginSeq;

		for (const auto& tileData : layer)
		{
			mOut << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(TextureID, tileData.textureID, mOut);
			LAB_SERIALISE_PROPERTY(Rotation, tileData.rotation, mOut);
			mOut << YAML::EndMap;
		}
		mOut << YAML::EndSeq;

		mOut << YAML::EndMap; // TilemapRenderLayer
	}
}

namespace YAML {

	template<>
	struct convert<Laby::TileRenderData>
	{
		inline static bool decode(const Node& node, Laby::TileRenderData& rhs)
		{
			auto tileData = node["TileData"];
			if (!tileData)
				return false;

			LAB_DESERIALISE_PROPERTY(TextureID, rhs.textureID, tileData);
			LAB_DESERIALISE_PROPERTY(Rotation, rhs.rotation, tileData);
		}
	};

	template<>
	struct convert<Laby::TilemapRenderLayer>
	{
		inline static bool decode(const Node& node, Laby::TilemapRenderLayer& rhs)
		{
			auto tilemapRenderLayer = node["TilemapRenderLayer"];
			if (!tilemapRenderLayer)
				return false;

			Laby::usize layer, width, height;
			LAB_DESERIALISE_PROPERTY(Layer, layer, tilemapRenderLayer);
			LAB_DESERIALISE_PROPERTY(Width, width, tilemapRenderLayer);
			LAB_DESERIALISE_PROPERTY(Height, height, tilemapRenderLayer);

			auto tiles = tilemapRenderLayer["Tiles"];
			if (!tiles)
				return false;

			rhs = Laby::TilemapRenderLayer(layer, width, height);

			Laby::usize index = 0;
			for (auto tile : tiles)
				rhs.set(index++, tile.as<Laby::TileRenderData>());

			return true;
		}
	};
}