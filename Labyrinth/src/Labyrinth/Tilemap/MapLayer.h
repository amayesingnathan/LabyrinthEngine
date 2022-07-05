#pragma once

#include <Labyrinth/Core/System/Assert.h>

namespace Labyrinth {

	class MapLayer
	{
	public:
		using TileID = size_t;

	public:
		MapLayer() = default;
		MapLayer(size_t width, size_t height) : mWidth(width), mHeight(height) { mTiles.reserve(width * height); }

		TileID& operator()(size_t x, size_t y) {return mTiles[x + (mWidth * y)]; }
		const TileID& operator()(size_t x, size_t y) const { return mTiles[x + (mWidth * y)]; }

		void add(TileID id) { mTiles.emplace_back(id); }

		size_t getWidth() const { return mWidth; }
		size_t getHeight() const { return mHeight; }

	private:
		std::vector<TileID> mTiles;
		size_t mWidth, mHeight;

	};

}