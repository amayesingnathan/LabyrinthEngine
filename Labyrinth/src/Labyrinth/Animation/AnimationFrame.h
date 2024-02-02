#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/IO/YAML.h>

namespace Laby {

	struct AnimationFrame
	{
		UUID id = 0;
		AssetHandle sprite = 0;
		usize length = 0;

		AnimationFrame() = default;
		AnimationFrame(AssetHandle subtex, usize frameLength)
			: sprite(subtex), length(frameLength) {}
		AnimationFrame(UUID id, AssetHandle subtex, usize frameLength)
			: id(id), sprite(subtex), length(frameLength) {}

		bool Valid() const { return id && sprite; }
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const AnimationFrame& data)
	{
		mOut << YAML::BeginMap; // AnimationFrame

		LAB_SERIALISE_PROPERTY(ID, data.id, mOut);
		LAB_SERIALISE_PROPERTY(Sprite, data.sprite, mOut);
		LAB_SERIALISE_PROPERTY(Length, data.length, mOut);

		mOut << YAML::EndMap; // AnimationFrame

		return mOut;
	}
}

namespace YAML {

	template<>
	struct convert<Laby::AnimationFrame>
	{
		inline static bool decode(const Node& node, Laby::AnimationFrame& rhs)
		{
			LAB_DESERIALISE_PROPERTY(ID, rhs.id, node);
			LAB_DESERIALISE_PROPERTY(Sprite, rhs.sprite, node);
			LAB_DESERIALISE_PROPERTY(Length, rhs.length, node);

			return true;
		}
	};
}