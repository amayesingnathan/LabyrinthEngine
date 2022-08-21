#pragma once

#include "TilemapTexture.h"

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>

namespace Labyrinth {

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

		bool hasSheet(AssetHandle sheet) const { return mTexture->hasSheet(sheet); }
		bool hasSheet(const Ref<Texture2DSheet>& sheet) const { return mTexture->hasSheet(sheet); }

		void addSheet(i32 id, AssetHandle sheet) { mTexture->addSheet(id, sheet); }
		void addSheet(i32 id, const Ref<Texture2DSheet>& sheet) { mTexture->addSheet(id, sheet); }

		void addLayer() { mTexture->addLayer({ mTexture->getLayers().size(), mWidth, mHeight }); }
		void addLayer(const TexMapLayer& layer) { mTexture->addLayer(layer); }

		bool moveLayer(usize index, LayerDirection direction) { return mTexture->moveLayer(index, direction); }

		void removeLayer(usize index) { mTexture->removeLayer(index); }

		void setTile(usize layer, TilePos pos, i32 id) 
		{ 
			mTexture->setTile(layer, pos, id); 
		}

		const std::unordered_map<TilePos, std::string>& getTileBehaviour() const { return mTileBehaviour; }
		void setTileBehaviour(TilePos pos, const std::string& script) { mTileBehaviour[pos] = script; }
		void removeTileBehaviour(TilePos pos) { if (mTileBehaviour.count(pos) != 0) mTileBehaviour.erase(pos); }

		static Ref<Tilemap> Create(const std::string& name, i32 width, i32 height) { return Ref<Tilemap>::Create(name, width, height); }
		static Ref<Tilemap> Create(const fs::path& path) { return Ref<Tilemap>::Create(path); }

	private:
		void RegenTexture() { mTexture->RegenTexture(); }

	private:
		std::string mMapName;

		Ref<TilemapTexture> mTexture = nullptr;
		i32 mWidth = 0, mHeight = 0;

		std::unordered_map<TilePos, std::string> mTileBehaviour;

		friend class TilemapSerialiser;
		friend class MapEditModal;
	};
}