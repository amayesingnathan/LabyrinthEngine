#include "Lpch.h"
#include "TilemapTexture.h"

#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Renderer/OrthographicCamera.h>

namespace Laby {

	TilemapTexture::TilemapTexture(u32 width, u32 height)
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

	TileRenderData TilemapTexture::getTile(usize layer, const GridPosition& pos) const
	{
		if (layer >= mLayers.size() || !pos.valid())
			return {};

		return mLayers[layer](pos);
	}

	void TilemapTexture::setTile(usize layer, const GridPosition& pos, const TileRenderData& data)
	{
		mLayers[layer](pos) = data;

		RenderTexture();
	}

	void TilemapTexture::removeLayer(usize layer)
	{
		auto layerIt = std::find_if(mLayers.begin(), mLayers.end(), [&](const TileRenderLayer& each) { return layer == each.getLayer(); });
		if (layerIt == mLayers.end())
			return;

		for (auto it = layerIt + 1; it != mLayers.end(); ++it)
			it->mIndex--;

		mLayers.erase(layerIt);
	}

	void TilemapTexture::moveLayer(usize layer, LayerMoveDir direction)
	{
		switch (direction)
		{
		case LayerMoveDir::Down:
		{
			if (layer == 0)
				return;
			break;
		}
		case LayerMoveDir::Up:
		{
			if (layer >= mLayers.size() - 1)
				return;
			break;
		}
		}

		usize newIndex = StaticCast<usize>((i64)layer + (i64)direction);

		std::swap(mLayers[layer], mLayers[newIndex]);
		usize tempIndex = mLayers[layer].mIndex;
		mLayers[layer].mIndex = mLayers[newIndex].mIndex;
		mLayers[newIndex].mIndex = tempIndex;
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
			for (u32 y = 0; y < mHeight; y++)
			{
				for (u32 x = 0; x < mWidth; x++)
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

	void TilemapTexture::UpdateLayers(const std::unordered_map<TileID, TileID>& mapping)
	{
		for (TileRenderLayer& layer : mLayers)
		{
			for (TileRenderData& tileData : layer)
				tileData.textureID = mapping.at(tileData.textureID);
		}
	}
}