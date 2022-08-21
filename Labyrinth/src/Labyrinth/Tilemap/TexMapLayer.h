#pragma once

#include <Labyrinth/Core/System/Assert.h>
#include <Labyrinth/IO/YAML.h>

namespace Labyrinth {

	class TexMapLayer
	{
	public:
		using TileID = i32;

	public:
		TexMapLayer() = default;
		TexMapLayer(usize layer, i32 width, i32 height) : mIndex(layer), mTiles(width * height, -1), mWidth(width), mHeight(height) { }

		TileID& operator()(usize x, usize y) 
		{
			return mTiles[x + (mWidth * y)]; 
		}
		const TileID& operator()(usize x, usize y) const { return mTiles[x + (mWidth * y)]; }

		bool operator==(const TexMapLayer& other) const { return mIndex == other.mIndex; }

		void add(TileID id) { mTiles.emplace_back(id); }
		void set(usize index, TileID id) { mTiles[index] = id; }

		i32 getWidth() const { return mWidth; }
		i32 getHeight() const { return mHeight; }
		usize getLayer() const { return mIndex; }

		void resize(usize size) { mTiles.reserve(size); }

		auto begin() { return mTiles.begin(); }
		auto begin() const { return mTiles.cbegin(); }
		auto end() { return mTiles.end(); }
		auto end() const { return mTiles.cend(); }

	private:
		usize mIndex = 0;
		std::vector<TileID> mTiles;
		i32 mWidth = 0, mHeight = 0;

		friend class TilemapTexture;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TexMapLayer& layer)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq;
		mOut << layer.getLayer() << layer.getWidth() << layer.getHeight();

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
			node.push_back(rhs.getLayer());
			node.push_back(rhs.getWidth());
			node.push_back(rhs.getHeight());
			for (auto id : rhs)
				node.push_back(id);
			return node;
		}

		inline static bool decode(const Node& node, Labyrinth::TexMapLayer& rhs)
		{
			if (!node.IsSequence())
				return false;

			auto it = node.begin();
			usize layer = it++->as<usize>();
			i32 width = it++->as<i32>();
			i32 height = it++->as<i32>();
			rhs = Labyrinth::TexMapLayer(layer, width, height);

			usize index = 0;
			for (it; it != node.end(); ++it, ++index)
				rhs.set(index, it->as<i32>());

			return true;
		}
	};
}