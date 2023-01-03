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

	private:
		void RenderTexture();

	private:
		usize mWidth = 0, mHeight = 0;
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