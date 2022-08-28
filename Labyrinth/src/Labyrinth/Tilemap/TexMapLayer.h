#pragma once

#include "TilePos.h"

#include <Labyrinth/Core/System/Assert.h>
#include <Labyrinth/IO/YAML.h>

namespace Labyrinth {

	using TileID = i32;
	struct TileData
	{
		TileID id = -1;
		i32 rotation = 0;

		TileData() = default;
		TileData(TileID _id, i32 _rotation) : id(_id), rotation(_rotation) {}

		bool operator==(const TileData& other) const { return id == other.id && rotation == other.rotation; }
		bool operator!=(const TileData& other) const { return !(*this == other); }
	};

	class TexMapLayer
	{
	public:
		TexMapLayer() = default;
		TexMapLayer(usize layer, i32 width, i32 height) : mIndex(layer), mTiles(width * height), mWidth(width), mHeight(height) { }

		TileData& operator[](const TilePos& pos) { return mTiles[pos.x + (mWidth * pos.y)]; }
		TileData operator[](const TilePos& pos) const
		{ 
			if (!pos.valid())
				return TileData();

			usize index = (usize)(pos.x + (mWidth * pos.y));
			if (index >= mTiles.size())
				return TileData();

			return mTiles[index];
		}

		bool operator==(const TexMapLayer& other) const { return mIndex == other.mIndex; }

		void add(TileID id, i32 rotation = 0) { mTiles.emplace_back(id, rotation); }
		void set(usize index, TileID id) { mTiles[index].id = id; }
		void set(usize index, TileID id, i32 rotation) 
		{ 
			mTiles[index].id = id; 
			mTiles[index].rotation = rotation;
		}

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
		std::vector<TileData> mTiles;
		i32 mWidth = 0, mHeight = 0;

		friend class TilemapTexture;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TexMapLayer& layer)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq;
		mOut << layer.getLayer() << layer.getWidth() << layer.getHeight();

		for (auto tileData : layer)
			mOut << tileData.id << tileData.rotation;

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
			for (auto tileData : rhs)
			{
				node.push_back(tileData.id);
				node.push_back(tileData.rotation);
			}
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
			for (it; it != node.end(); ++index)
			{
				i32 id = it++->as<i32>();
				i32 rotation = it++->as<i32>();
				rhs.set(index, id, rotation);
			}

			return true;
		}
	};
}