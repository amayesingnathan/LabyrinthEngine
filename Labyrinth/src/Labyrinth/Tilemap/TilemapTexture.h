#pragma once

#include "TexMapLayer.h"

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/SubTexture.h>
#include <Labyrinth/Renderer/Framebuffer.h>
#include <Labyrinth/Renderer/OrthographicCamera.h>

namespace Labyrinth {

	struct SheetData
	{
		i32 firstID = 0;
		AssetHandle sheet = 0;

		SheetData() = default;
		SheetData(i32 id, AssetHandle tex) : firstID(id), sheet(tex) {}

		bool operator <(const SheetData& other) const
		{
			return firstID < other.firstID;
		}
	};

	enum class LayerDirection
	{
		Up, Down
	};

	class TilemapTexture : public RefCounted
	{
	public:
		TilemapTexture(i32 width, i32 height);
		TilemapTexture(const fs::path& path);

		const Ref<Framebuffer>& getTex() const { return mTexture; }

		const std::vector<SheetData>& getSheets() const { return mSheets; }
		const std::vector<TexMapLayer>& getLayers() const { return mLayers; }

		void addSheet(i32 id, AssetHandle sheet) { mSheets.emplace_back(id, sheet); }
		void addSheet(i32 id, const Ref<Texture2DSheet>& sheet) { mSheets.emplace_back(id, sheet->handle); }

		bool hasSheet(AssetHandle sheet) const
		{
			return std::find_if(mSheets.begin(), mSheets.end(), [&sheet](const SheetData& data) { return sheet == data.sheet; }) != mSheets.end();
		}
		bool hasSheet(const Ref<Texture2DSheet>& sheet) const 
		{ 
			return std::find_if(mSheets.begin(), mSheets.end(), [&sheet](const SheetData& data) { return sheet->handle == data.sheet; }) != mSheets.end();
		}

		void addLayer(const TexMapLayer& layer) { mLayers.emplace_back(layer); }

		bool moveLayer(usize index, LayerDirection direction);

		void removeLayer(usize index) 
		{ 
			mLayers.erase(std::remove_if(mLayers.begin(), mLayers.end(), 
				[=](const TexMapLayer& layer) { return layer.getLayer() == index; }),
				mLayers.end());
		}

		i32 getTile(usize layer, const TilePos& pos) const;
		void setTile(usize layer, TilePos pos, i32 id);

		Ref<SubTexture2D> getTileTex(i32 tileID) const;

		static Ref<TilemapTexture> Create(i32 width, i32 height) { return Ref<TilemapTexture>::Create(width, height); }
		static Ref<TilemapTexture> Create(const fs::path& path) { return Ref<TilemapTexture>::Create(path); }

	private:
		void RegenTexture();
		AssetHandle GetSheet(i32 tileID) const;

	private:
		i32 mWidth = 0, mHeight = 0;
		std::vector<SheetData> mSheets;
		std::vector<TexMapLayer> mLayers;
		Ref<Framebuffer> mTexture = nullptr;
		OrthographicCamera mCamera;

		friend class Tilemap;
	};

}