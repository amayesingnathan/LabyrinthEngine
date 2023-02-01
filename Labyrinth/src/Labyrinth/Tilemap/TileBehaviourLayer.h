#pragma once

#include <ranges>

#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Scripting/ScriptEngine.h>

namespace Laby {

	struct TileBehaviourData
	{
		bool solid = false;
		std::string_view script = "";
	};

	using TileBehaviourGrid = Grid<TileBehaviourData>;
	using Shape = std::vector<GridPosition>;

	struct TileScriptData
	{
		GridPosition pos;
		std::string_view script;

		TileScriptData(u32 x, u32 y, std::string_view behaviour)
			: pos(x, y), script(behaviour) {}
	};

	struct ChainShape
	{
		glm::vec2* vertices = nullptr;
		i32 vertexCount = 0;
		glm::vec2 extents;

		ChainShape(const Shape& shape, const glm::vec2& ex);
		glm::vec2 centroid() const;
	};

	class TileBehaviourLayer : public TileBehaviourGrid
	{
	public:
		TileBehaviourLayer() = default;
		TileBehaviourLayer(usize width, usize height)
			: TileBehaviourGrid(width, height) {}

		std::vector<ChainShape> getShapes() const;
		std::vector<TileScriptData> getScripts() const;

	private:
		std::vector<Shape> GetContiguousShapes() const;
		Shape FloodFill(u32 x, u32 y, Grid<Bool>& checkedGrid) const;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileBehaviourData& data)
	{
		mOut << YAML::BeginMap; // TileData

		LAB_SERIALISE_PROPERTY(Solid, data.solid, mOut);
		LAB_SERIALISE_PROPERTY(Script, data.script.data(), mOut);

		mOut << YAML::EndMap; // TileData

		return mOut;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileBehaviourLayer& layer)
	{
		mOut << YAML::BeginMap; // TileBehaviourLayer

		LAB_SERIALISE_PROPERTY(Width, layer.getWidth(), mOut);
		LAB_SERIALISE_PROPERTY(Height, layer.getHeight(), mOut);

		mOut << YAML::Key << "Tiles";
		mOut << YAML::Value << YAML::BeginSeq;

		for (const auto& tileData : layer)
		{
			mOut << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(Solid, tileData.solid, mOut);
			LAB_SERIALISE_PROPERTY(Script, tileData.script.data(), mOut);
			mOut << YAML::EndMap;
		}
		mOut << YAML::EndSeq;

		mOut << YAML::EndMap; // TileBehaviourLayer

		return mOut;
	}
}

namespace YAML {

	template<>
	struct convert<Laby::TileBehaviourData>
	{
		inline static bool decode(const Node& node, Laby::TileBehaviourData& rhs)
		{
			std::string script;
			LAB_DESERIALISE_PROPERTY(Solid, rhs.solid, node);
			LAB_DESERIALISE_PROPERTY(Script, script, node);

			auto classNames = std::views::keys(Laby::ScriptEngine::GetAppClasses());
			auto it = std::ranges::find(classNames, script);
			if (it != classNames.end())
				rhs.script = *it;

			return true;
		}
	};

	template<>
	struct convert<Laby::TileBehaviourLayer>
	{
		inline static bool decode(const Node& node, Laby::TileBehaviourLayer& rhs)
		{
			Laby::usize width, height;
			LAB_DESERIALISE_PROPERTY(Width, width, node);
			LAB_DESERIALISE_PROPERTY(Height, height, node);

			auto tiles = node["Tiles"];
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