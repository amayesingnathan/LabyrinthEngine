#pragma once

#include <Labyrinth/IO/YAML.h>

#include <string>

namespace Labyrinth {

	struct TileSpec
	{
		std::string script;
		bool solid = false;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TileSpec& spec)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << spec.script << spec.solid << YAML::EndSeq;
		return mOut;
	}
}

namespace YAML {

	template<>
	struct convert<Labyrinth::TileSpec>
	{
		inline static Node encode(const Labyrinth::TileSpec& rhs)
		{
			Node node;
			node.push_back(rhs.script);
			node.push_back(rhs.solid);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Labyrinth::TileSpec& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.script = node[0].as<std::string>();
			rhs.solid = node[1].as<bool>();
			return true;
		}
	};
}