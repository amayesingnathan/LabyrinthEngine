#pragma once

#include <Labyrinth/Core/System/Assert.h>
#include <Labyrinth/IO/YAML.h>

namespace Labyrinth {

	class TexMapLayer
	{
	public:
		using TileID = usize;

	public:
		TexMapLayer() = default;
		TexMapLayer(i32 width, i32 height) : mWidth(width), mHeight(height) { mTiles.reserve(width * height); }

		TileID& operator()(usize x, usize y) {return mTiles[x + (mWidth * y)]; }
		const TileID& operator()(usize x, usize y) const { return mTiles[x + (mWidth * y)]; }

		void add(TileID id) { mTiles.emplace_back(id); }

		i32 getWidth() const { return mWidth; }
		i32 getHeight() const { return mHeight; }

		void resize(usize size) { mTiles.reserve(size); }

		auto begin() { return mTiles.begin(); }
		auto begin() const { return mTiles.cbegin(); }
		auto end() { return mTiles.end(); }
		auto end() const { return mTiles.cend(); }

	private:
		std::vector<TileID> mTiles;
		i32 mWidth, mHeight;

	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TexMapLayer& layer)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq;
		for (auto id : layer)
			mOut << id;

		mOut << YAML::EndSeq;
		return mOut;
	}
}

namespace YAML {

	template<>
	struct convert<Labyrinth::TexMapLayer>
	{
		inline static Node encode(const Labyrinth::TexMapLayer& rhs)
		{
			Node node;
			for (auto id : rhs)
				node.push_back(id);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Labyrinth::TexMapLayer& rhs)
		{
			if (!node.IsSequence())
				return false;

			rhs.resize(node.size());
			for (auto element : node)
				rhs.add(element.as<usize>());

			return true;
		}
	};
}