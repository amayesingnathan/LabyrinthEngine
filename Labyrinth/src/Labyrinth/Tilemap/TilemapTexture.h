#pragma once

#include "TexMapLayer.h"

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Labyrinth {

	struct SheetData
	{
		usize firstID;
		AssetHandle sheet;

		SheetData(usize id, AssetHandle tex) : firstID(id), sheet(tex) {}

		bool operator <(const SheetData& other) const
		{
			return firstID < other.firstID;
		}
	};

	class TilemapTexture : public RefCounted
	{
	public:
		TilemapTexture(i32 width, i32 height);
		TilemapTexture(const fs::path& path);

		const Ref<Texture2D>& getTex() const { return mTexture; }

		const std::vector<SheetData>& getSheets() const { return mSheets; }
		const std::vector<TexMapLayer>& getLayers() const { return mLayers; }

		void addSheet(usize id, AssetHandle sheet) { mSheets.emplace_back(id, sheet); }
		void addSheet(usize id, const Ref<Texture2DSheet>& sheet) { mSheets.emplace_back(id, sheet->handle); }

		void addLayer(const TexMapLayer& layer) { mLayers.emplace_back(layer); }

		void setTile(usize layer, usize x, usize y, usize id) { mLayers[layer](x, y) = id; }

		static Ref<TilemapTexture> Create(i32 width, i32 height) { return Ref<TilemapTexture>::Create(width, height); }
		static Ref<TilemapTexture> Create(const fs::path& path) { return Ref<TilemapTexture>::Create(path); }

	private:
		void RegenTexture();
		AssetHandle GetSheet(usize tileID) const;
		usize GetID(AssetHandle handle) const;

	private:
		i32 mWidth = 0, mHeight = 0;
		std::vector<SheetData> mSheets;
		std::vector<TexMapLayer> mLayers;
		Ref<Texture2D> mTexture = nullptr;

		friend class Tilemap;
	};

}