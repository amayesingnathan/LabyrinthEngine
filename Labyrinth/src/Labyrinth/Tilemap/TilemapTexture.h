#pragma once

#include <Labyrinth/Renderer/Framebuffer.h>
#include <Labyrinth/Renderer/OrthographicCamera.h>

#include "TileRenderLayer.h"
#include "TilePalette.h"

namespace Laby {

	enum class LayerMoveDir
	{
		Down = -1,
		Up = 1
	};

	class TilemapTexture
	{
	public:
		TilemapTexture(u32 width, u32 height);

		void AddSheet(AssetHandle handle) { mTilePalette.Add(handle); }
		void AddSheet(AssetHandle handle, TileID startIndex) { mTilePalette.Add(handle, startIndex); }
		void RemoveSheet(AssetHandle handle);

		Ref<SubTexture2D> GetTileTex(TileID id) const { return mTilePalette[id]; }

		TileRenderData GetTile(usize layer, const GridPosition& pos) const;
		void SetTile(usize layer, const GridPosition& pos, const TileRenderData& renderData);

		bool HasLayers() const { return !mLayers.empty(); }

		void AddLayer() { mLayers.emplace_back(mWidth, mHeight); RenderTexture(); }
		void RemoveLayer(usize layer);
		void MoveLayer(usize layer, LayerMoveDir direction);

		const std::vector<TileRenderLayer>& GetLayers() const { return mLayers; }
		const std::vector<SheetData>& GetSheets() const { return mTilePalette.GetSheets(); }

	private:
		void AddLayer(const TileRenderLayer& layer) { mLayers.emplace_back(layer); RenderTexture();}
		void AddLayer(TileRenderLayer&& layer) { mLayers.emplace_back(std::move(layer)); RenderTexture();}

	private:
		void RenderTexture();
		void UpdateLayers(const std::unordered_map<TileID, TileID>& mapping);

	private:
		const u32 mWidth = 0, mHeight = 0;
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