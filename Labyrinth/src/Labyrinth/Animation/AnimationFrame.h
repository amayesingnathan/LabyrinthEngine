#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/IO/YAML.h>

namespace Laby {

	struct AnimationFrame
	{
		AssetHandle sprite = 0;
		usize frameLength = 8;

		AnimationFrame() = default;
		AnimationFrame(AssetHandle subtex, usize length)
			: sprite(subtex), frameLength(length) {}

		bool valid() const { return sprite && frameLength != 0; }
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const AnimationFrame& data)
	{
		mOut << YAML::BeginMap; // AnimationFrame

		LAB_SERIALISE_PROPERTY(Sprite, data.sprite, mOut);
		LAB_SERIALISE_PROPERTY(Length, data.frameLength, mOut);

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
			LAB_DESERIALISE_PROPERTY(Sprite, rhs.sprite, node);
			LAB_DESERIALISE_PROPERTY(Length, rhs.frameLength, node);

			return true;
		}
	};
}