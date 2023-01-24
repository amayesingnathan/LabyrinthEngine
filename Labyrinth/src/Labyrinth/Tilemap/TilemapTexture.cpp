#include "Lpch.h"
#include "TilemapTexture.h"

#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Renderer/OrthographicCamera.h>

namespace Laby {

	TilemapTexture::TilemapTexture(usize width, usize height)
		: mWidth(width), mHeight(height), mCamera(0.0f, StaticCast<f32>(mWidth * TileSize.x), 0.0f, StaticCast<f32>(mHeight * TileSize.y))
	{
		FramebufferSpec fbSpec;
		fbSpec.width = (i32)mWidth * TileSize.x;
		fbSpec.height = (i32)mHeight * TileSize.y;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Ref<Framebuffer>::Create(fbSpec);
	}

	void TilemapTexture::removeSheet(AssetHandle handle)
	{
		std::unordered_map<TileID, TileID> tileMapping;
		mTilePalette.remove(handle, tileMapping);
		UpdateLayers(tileMapping);
		RenderTexture();
	}

	TileID TilemapTexture::getTile(usize layer, const GridPosition& pos)
	{
		return mLayers[layer](pos).textureID;
	}

	void TilemapTexture::setTile(usize layer, const GridPosition& pos, TileID tile, f32 rotation)
	{
		TileRenderData& tileData = mLayers[layer](pos);
		tileData.textureID = tile;
		tileData.rotation = rotation;

		RenderTexture();
	}

	void TilemapTexture::RenderTexture()
	{
		mFramebuffer->bind();

		Renderer::SetClearColor({ 0.f, 0.f, 0.f, 1 });
		Renderer::Clear();
		Renderer::DisableDepth();

		Renderer2D::BeginState(mCamera);

		for (const TileRenderLayer& layer : mLayers)
		{
			for (usize y = 0; y < mHeight; y++)
			{
				for (usize x = 0; x < mWidth; x++)
				{
					const auto& tileData = layer(x, y);
					Ref<SubTexture2D> tex = mTilePalette[tileData.textureID];
					if (!tex)
						continue;

					glm::vec2 pos = { x * TileSize.x, y * TileSize.y };
					pos += 0.5f * TileSizeF;
					Renderer2D::DrawRotatedQuad(pos, TileSizeF, tileData.rotation, tex);
				}
			}
		}

		Renderer2D::EndState();
		Renderer::EnableDepth();
		mFramebuffer->unbind();
	}

	void TilemapTexture::UpdateLayers(const std::unordered_map<TileID, TileID> mapping)
	{
		for (TileRenderLayer& layer : mLayers)
		{
			for (TileRenderData& tileData : layer)
				tileData.textureID = mapping.at(tileData.textureID);
		}
	}
}