#pragma once

#include <Labyrinth/IO/YAML.h>

namespace Labyrinth {

	struct TilePos
	{
		i32 x = -1, y = -1;

		TilePos() = default;
		TilePos(i32 _x, i32 _y) : x(_x), y(_y) {}
		TilePos(usize _x, usize _y) : x((i32)_x), y((i32)_y) {}

		bool operator==(const TilePos& other) const { return x == other.x && y == other.y; }
		bool valid() const { return x > 0 && y > 0; }

		std::string toString() const { return fmt::format("({}, {})", x, y); }
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TilePos& pos)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << pos.x << pos.y << YAML::EndSeq;
		return mOut;
	}
}


namespace std {
	template<typename T> struct hash;

	template<>
	struct hash<Labyrinth::TilePos>
	{
		std::size_t operator()(const Labyrinth::TilePos& tilePos) const
		{
			u64 combination = (u64)tilePos.x | ((u64)tilePos.y << 32);
			return std::hash<u64>()(combination);
		}
	};
}

namespace YAML {

	template<>
	struct convert<Labyrinth::TilePos>
	{
		inline static Node encode(const Labyrinth::TilePos& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Labyrinth::TilePos& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<i32>();
			rhs.y = node[1].as<i32>();
			return true;
		}
	};
}