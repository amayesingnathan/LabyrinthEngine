#pragma once

#include <Labyrinth/Renderer/Framebuffer.h>
#include <Labyrinth/Renderer/OrthographicCamera.h>

#include "TileRenderLayer.h"
#include "TilePalette.h"

namespace Laby {

	class TilemapTexture
	{
	public:
		TilemapTexture(usize width, usize height);

		void addSheet(AssetHandle handle) { mTilePalette.add(handle); }
		void addSheet(AssetHandle handle, TileID startIndex) { mTilePalette.add(handle, startIndex); }
		void removeSheet(AssetHandle handle);

		Ref<SubTexture2D> getTileTex(TileID id) const { return mTilePalette[id]; }

		const TileRenderData& getTile(usize layer, const GridPosition& pos) const { return mLayers[layer](pos); }
		void setTile(usize layer, const GridPosition& pos, TileID tile, f32 rotation);

		void addLayer() { mLayers.emplace_back(mLayers.size(), mWidth, mHeight); }

		const std::vector<TileRenderLayer>& getLayers() const { return mLayers; }
		const std::vector<SheetData>& getSheets() const { return mTilePalette.getSheets(); }

	private:
		void addLayer(const TileRenderLayer& layer) { mLayers.emplace_back(layer); RenderTexture();}
		void addLayer(TileRenderLayer&& layer) { mLayers.emplace_back(std::move(layer)); RenderTexture();}

	private:
		void RenderTexture();
		void UpdateLayers(const std::unordered_map<TileID, TileID>& mapping);

	private:
		const usize mWidth = 0, mHeight = 0;
		std::vector<TileRenderLayer> mLayers;
		TilePalette mTilePalette;

		Ref<Framebuffer> mFramebuffer = nullptr;
		OrthographicCamera mCamera;

		friend class Tilemap;
		friend class TilemapUtils;

	private:
		static constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };
		static constexpr glm::vec2 TileSizeF = glm::vec2{ 64.0f };
	};
}