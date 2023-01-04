#pragma once

#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/YAML.h>

struct b2Vec2;

namespace Laby {

	struct TileBehaviourData
	{
		bool solid;
		std::string script;
	};

	using TileBehaviourGrid = Grid<TileBehaviourData>;
	using GridPos = TileBehaviourGrid::GridPos;
	using Shape = std::vector<GridPos>;

	struct ChainShape
	{
		glm::vec2* vertices = nullptr;
		i32 vertexCount = 0;

		ChainShape(const Shape& shape);
		void free() { delete[] vertices; }
		const b2Vec2* get() const { return StaticCast<b2Vec2>(vertices); }
	};

	class TileBehaviourLayer : public TileBehaviourGrid
	{
	public:

	public:
		TileBehaviourLayer(usize width, usize height)
			: TileBehaviourGrid(width, height) {}

		std::vector<ChainShape> getShapes() const;

	private:
		f32 GetAngle(const GridPos& p, const GridPos& q) const;
		f32 GetDistance(const GridPos& p, const GridPos& q) const;

		ChainShape TraverseShape(const Shape& shape) const;
		std::vector<GridPos> GetEdgeVertices(const Shape& vertices) const;
		std::vector<Shape> GetContiguousShapes() const;
		Shape FloodFill(usize x, usize y, Grid<bool>& checkedGrid) const;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileBehaviourData& data)
	{
		mOut << YAML::Key << "TileData";
		mOut << YAML::BeginMap; // TileData

		LAB_SERIALISE_PROPERTY(Solid, data.solid, mOut);
		LAB_SERIALISE_PROPERTY(Script, data.script, mOut);

		mOut << YAML::EndMap; // TileData
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileBehaviourLayer& layer)
	{
		mOut << YAML::Key << "TileBehaviourLayer";
		mOut << YAML::BeginMap; // TileBehaviourLayer

		LAB_SERIALISE_PROPERTY(Width, layer.getWidth(), mOut);
		LAB_SERIALISE_PROPERTY(Height, layer.getHeight(), mOut);

		mOut << YAML::Key << "Tiles";
		mOut << YAML::Value << YAML::BeginSeq;

		for (const auto& tileData : layer)
		{
			mOut << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(Solid, tileData.solid, mOut);
			LAB_SERIALISE_PROPERTY(Script, tileData.script, mOut);
			mOut << YAML::EndMap;
		}
		mOut << YAML::EndSeq;

		mOut << YAML::EndMap; // TileBehaviourLayer
	}
}

namespace YAML {

	template<>
	struct convert<Laby::TileBehaviourData>
	{
		inline static bool decode(const Node& node, Laby::TileBehaviourData& rhs)
		{
			auto tileData = node["TileData"];
			if (!tileData)
				return false;

			LAB_DESERIALISE_PROPERTY(Solid, rhs.solid, tileData);
			LAB_DESERIALISE_PROPERTY(Script, rhs.script, tileData);
		}
	};

	template<>
	struct convert<Laby::TileBehaviourLayer>
	{
		inline static bool decode(const Node& node, Laby::TileBehaviourLayer& rhs)
		{
			auto TileBehaviourLayer = node["TileBehaviourLayer"];
			if (!TileBehaviourLayer)
				return false;

			Laby::usize layer, width, height;
			LAB_DESERIALISE_PROPERTY(Width, width, TileBehaviourLayer);
			LAB_DESERIALISE_PROPERTY(Height, height, TileBehaviourLayer);

			auto tiles = TileBehaviourLayer["Tiles"];
			if (!tiles)
				return false;

			rhs = Laby::TileBehaviourLayer(width, height);

			Laby::usize index = 0;
			for (auto tile : tiles)
				rhs.set(index++, tile.as<Laby::TileBehaviourData>());

			return true;
		}
	};
}