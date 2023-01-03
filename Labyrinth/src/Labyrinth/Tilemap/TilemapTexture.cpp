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

	void TilemapTexture::RenderTexture()
	{
		mFramebuffer->bind();

		Renderer::SetClearColor({ 0.f, 0.f, 0.f, 1 });
		Renderer::Clear();
		Renderer::DisableDepth();

		Renderer2D::BeginState(mCamera);

		for (const TilemapRenderLayer& layer : mLayers)
		{
			for (usize y = 0; y < mHeight; y++)
			{
				for (usize x = 0; x < mWidth; x++)
				{
					glm::vec2 pos = { x * TileSize.x, y * TileSize.y };
					pos += 0.5f * TileSizeF;

					const auto& tileData = layer(x, y);
					Renderer2D::DrawRotatedQuad(pos, TileSizeF, tileData.rotation, mTilePalette[tileData.textureID]);
				}
			}
		}

		Renderer2D::EndState();
		Renderer::EnableDepth();
		mFramebuffer->unbind();
	}
}