#pragma once

#include "TilemapTexture.h"

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>

namespace Labyrinth {

	struct Tile
	{
		usize x, y;

		Tile(usize _x, usize _y) : x(_x), y(_y) {}

		bool operator ==(const Tile& other) const { return x == other.x && y == other.y; }
		bool operator <(const Tile& other) const
		{ 
			if (y != other.y)
				return y < other.y;

			return x < other.x;
		}
	};

	class Tilemap : public Asset
	{
	public:
		Tilemap(const std::string& name, usize width, usize height);
		Tilemap(const fs::path& path, usize width, usize height);

		const Ref<Texture2D>& getTex() const { return mTexture->getTex(); };

		static Ref<Tilemap> Create(const std::string& name, usize width, usize height) { return Ref<Tilemap>::Create(name, width, height); }
		static Ref<Tilemap> Create(const fs::path& path, usize width, usize height) { return Ref<Tilemap>::Create(path, width, height); }

	private:
		void RegenTexture() { mTexture->RegenTexture(); }
		void RegenEntities();

	private:
		std::string mMapName;
		Entity mMapEntity;

		Ref<Scene> mContext = nullptr;
		Ref<TilemapTexture> mTexture = nullptr;
		usize mWidth = 0, mHeight = 0;

		std::map<Tile, Entity> mEntityMap;

		friend class TilemapManager;
	};
}