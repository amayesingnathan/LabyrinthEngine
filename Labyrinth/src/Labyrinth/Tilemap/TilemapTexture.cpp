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

	bool TilemapTexture::moveLayer(usize index, LayerDirection direction)
	{
		LAB_CORE_ASSERT(index < mLayers.size());

		i32 indexDirection = 0;

		switch (direction)
		{
		case LayerDirection::Up:
		{
			if (index == 0) break;

			indexDirection = -1;
		}
		break;
		case LayerDirection::Down:
		{
			if (index == mLayers.size() - 1) break;

			indexDirection = 1;
		}
		break;
		}

		if (indexDirection == 0)
			return false;

		std::swap(mLayers[index], mLayers[index + indexDirection]);
		usize tempLayer = mLayers[index].mIndex;
		mLayers[index].mIndex = mLayers[index + indexDirection].mIndex;
		mLayers[index + indexDirection].mIndex = tempLayer;

		return true;
	}

	void TilemapTexture::RegenTexture()
	{
		// TODO: maybe too large, estimate 100x100 map would be ~150MB of texture memory.
		constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };
		constexpr glm::vec2 TileSizeF = glm::vec2{ 64.0f };

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

		OrthographicCamera camera(0.0f, Cast<f32>(fbSpec.width), 0.0f, Cast<f32>(fbSpec.height));
		Renderer2D::BeginState(camera);

		for (const TexMapLayer& layer : mLayers)
		{
			for (usize y = 0; y < mHeight; y++)
			{
				for (usize x = 0; x < mWidth; x++)
				{
					i32 tileID = layer[{x, mHeight - y - 1}];
					if (tileID == -1) continue;

					Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(GetSheet(tileID));
					glm::vec2 pos = { x * TileSize.x, y * TileSize.y };
					pos += 0.5f * TileSizeF;

					Renderer2D::DrawQuad(pos, TileSizeF, sheet->getSubTex(tileID));
				}
			}
		}

		Renderer2D::EndState();
		RenderCommand::EnableDepth();

		mTexture = Texture2D::Create(fbSpec.width, fbSpec.height);

		Buffer texData(4 * fbSpec.width * fbSpec.height);
		textureFB->readData(0, texData.data);
		mTexture->setData(texData);

		textureFB->unbind();
	}

	i32 TilemapTexture::getTile(usize layer, const TilePos& pos) const
	{
		if (layer >= mLayers.size())
			return -1;

		return mLayers[layer][pos];
	}

	void TilemapTexture::setTile(usize layer, TilePos pos, i32 id)
	{
		if (layer >= mLayers.size())
			return;

		i32& tileID = mLayers[layer][pos];
		tileID = id;
		RegenTexture();
	}

	Ref<SubTexture2D> TilemapTexture::getTileTex(i32 tileID) const
	{
		if (tileID < 0)
			return nullptr;

		Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(GetSheet(tileID));
		if (!sheet)
			return nullptr;

		return sheet->getSubTex(tileID);
	}

	AssetHandle TilemapTexture::GetSheet(i32 tileID) const
	{
		auto it = std::find_if(mSheets.rbegin(), mSheets.rend(), [tileID](const SheetData& sheetData) { return sheetData.firstID <= tileID; });

		if (it != mSheets.rend())
			return it->sheet;

		return 0;
	}
}