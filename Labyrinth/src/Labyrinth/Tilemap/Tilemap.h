#pragma once

#include "TilemapTexture.h"

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>

namespace Labyrinth {

	struct TilePos
	{
		i32 x = -1, y = -1;

		TilePos() = default;
		TilePos(i32 _x, i32 _y) : x(_x), y(_y) {}

		bool operator==(const TilePos& other) const { return x == other.x && y == other.y; }
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const TilePos& pos)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << pos.x << pos.y << YAML::EndSeq;
		return mOut;
	}

	class Tilemap : public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::Tilemap)

	public:
		Tilemap(const std::string& name, i32 width, i32 height);
		Tilemap(const fs::path& path);

		const Ref<Texture2D>& getTex() const { return mTexture->getTex(); };

		const std::string& getName() { return mMapName; }
		i32 getWidth() const { return mWidth; }
		i32 getHeight() const { return mHeight; }

		const std::vector<SheetData>& getSheets() const { return mTexture->getSheets(); }
		const std::vector<TexMapLayer>& getLayers() const { return mTexture->getLayers(); }

		void addSheet(usize id, AssetHandle sheet) { mTexture->addSheet(id, sheet); }
		void addSheet(usize id, const Ref<Texture2DSheet>& sheet) { mTexture->addSheet(id, sheet); }

		void addLayer(const TexMapLayer& layer) { mTexture->addLayer(layer); }

		static Ref<Tilemap> Create(const std::string& name, i32 width, i32 height) { return Ref<Tilemap>::Create(name, width, height); }
		static Ref<Tilemap> Create(const fs::path& path) { return Ref<Tilemap>::Create(path); }

	private:
		void RegenTexture() { mTexture->RegenTexture(); }

	private:
		std::string mMapName;

		Ref<TilemapTexture> mTexture = nullptr;
		i32 mWidth = 0, mHeight = 0;

		friend class TilemapManager;
	};
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