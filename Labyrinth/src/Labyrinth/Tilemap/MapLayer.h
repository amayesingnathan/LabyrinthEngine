#pragma once

#include <Labyrinth/Core/System/Assert.h>

namespace Labyrinth {

	class MapLayer
	{
	public:
		using TileID = usize;

	public:
		MapLayer() = default;
		MapLayer(usize width, usize height) : mWidth(width), mHeight(height) { mTiles.reserve(width * height); }

		TileID& operator()(usize x, usize y) {return mTiles[x + (mWidth * y)]; }
		const TileID& operator()(usize x, usize y) const { return mTiles[x + (mWidth * y)]; }

		void add(TileID id) { mTiles.emplace_back(id); }

		usize getWidth() const { return mWidth; }
		usize getHeight() const { return mHeight; }

	private:
		std::vector<TileID> mTiles;
		usize mWidth, mHeight;

	};

}