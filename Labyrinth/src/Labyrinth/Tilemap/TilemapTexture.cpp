#include "Lpch.h"
#include "TilemapTexture.h"

#include "Labyrinth/Assets/AssetManager.h"
#include "Labyrinth/IO/Tiled.h"
#include <Labyrinth/Renderer/Framebuffer.h>
#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

namespace Labyrinth {

	TilemapTexture::TilemapTexture(i32 width, i32 height)
		: mWidth(width), mHeight(height)
	{
	}

	TilemapTexture::TilemapTexture(const fs::path& path)
	{
		TiledIO::Open(path, mLayers, mSheets);
		if (!mLayers.empty())
		{
			mWidth = mLayers[0].getWidth();
			mHeight = mLayers[0].getHeight();
		}
	}

	void TilemapTexture::RegenTexture()
	{
		// TODO: maybe too large, estimate 100x100 map would be ~150MB of texture memory.
		constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };
		constexpr glm::vec2 TileSizeF = glm::vec2{ 64.0f };
		constexpr glm::vec2 TileTexSize = 2.0f / TileSizeF;

		FramebufferSpec fbSpec;
		fbSpec.width = mWidth * TileSize.x;
		fbSpec.height = mHeight * TileSize.y;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		Ref<Framebuffer> textureFB = Framebuffer::Create(fbSpec);
		textureFB->bind();

		RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1 });
		RenderCommand::Clear();
		RenderCommand::DisableDepth();

		Renderer2D::BeginState();

		for (const TexMapLayer& layer : mLayers)
		{
			for (usize y = 0; y < mHeight; y++)
			{
				for (usize x = 0; x < mWidth; x++)
				{
					i32 tileID = layer(x, mHeight - y - 1);
					if (tileID == -1) continue;

					Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(GetSheet(tileID));
					glm::vec2 pos = { x * TileSize.x, y * TileSize.y };
					//pos -= 1; TODO: Not sure why this is here 

					Renderer2D::DrawQuad(pos, TileTexSize, sheet->getSubTex(tileID));
				}
			}
		}

		Renderer2D::EndState();
		RenderCommand::EnableDepth();

		mTexture = Texture2D::Create(fbSpec.width, fbSpec.height);

		Buffer texData(4 * (mWidth * TileSize.x) * (mHeight * TileSize.y));
		textureFB->readData(0, texData.data);
		mTexture->setData(texData);

		textureFB->unbind();
	}

	AssetHandle TilemapTexture::GetSheet(usize tileID) const
	{
		auto it = std::find_if(mSheets.rbegin(), mSheets.rend(), [tileID](const SheetData& sheetData) { return sheetData.firstID <= tileID; });

		if (it != mSheets.rend())
			return it->sheet;

		return 0;
	}
}