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

		void addLayer() { mLayers.emplace_back(mLayers.size(), mWidth, mHeight); }

		const std::vector<TileRenderLayer>& getLayers() const { return mLayers; }
		const std::unordered_set<AssetHandle>& getSheets() const { return mTilePalette.getSheets(); }

	private:
		void addLayer(TileRenderLayer&& layer) { mLayers.emplace_back(std::move(layer)); RenderTexture();}

	private:
		void RenderTexture();

	private:
		const usize mWidth = 0, mHeight = 0;
		std::vector<TileRenderLayer> mLayers;
		TilePalette mTilePalette;

		Ref<Framebuffer> mFramebuffer = nullptr;
		OrthographicCamera mCamera;

		friend class Tilemap;

	private:
		static constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };
		static constexpr glm::vec2 TileSizeF = glm::vec2{ 64.0f };
	};
}