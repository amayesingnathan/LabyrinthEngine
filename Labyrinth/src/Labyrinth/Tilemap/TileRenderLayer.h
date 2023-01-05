#pragma once

#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/YAML.h>

namespace Laby {

	struct TileRenderData
	{
		usize textureID;
		f32 rotation;
	};

	class TileRenderLayer : public Grid<TileRenderData>
	{
	public:
		TileRenderLayer() = default;
		TileRenderLayer(usize layer, usize width, usize height)
			: Grid<TileRenderData>(width, height), mIndex(layer) {}

		bool operator==(const TileRenderLayer& other) const { return mIndex == other.mIndex; }

		usize getLayer() const { return mIndex; }

	private:
		usize mIndex = 0;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileRenderData& data)
	{
		mOut << YAML::BeginMap; // TileData
		LAB_SERIALISE_PROPERTY(TextureID, data.textureID, mOut);
		LAB_SERIALISE_PROPERTY(Rotation, data.rotation, mOut);
		mOut << YAML::EndMap; // TileData
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileRenderLayer& layer)
	{
		mOut << YAML::BeginMap; // TileRenderLayer

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
			Laby::usize layer, width, height;
			LAB_DESERIALISE_PROPERTY(Layer, layer, node);
			LAB_DESERIALISE_PROPERTY(Width, width, node);
			LAB_DESERIALISE_PROPERTY(Height, height, node);

			auto tiles = node["Tiles"];
			if (!tiles)
				return false;

			rhs = Laby::TileRenderLayer(layer, width, height);

			Laby::usize index = 0;
			for (auto tile : tiles)
				rhs.set(index++, tile.as<Laby::TileRenderData>());

			return true;
		}
	};
}