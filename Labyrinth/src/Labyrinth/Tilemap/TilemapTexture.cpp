#include "Lpch.h"
#include "TilemapTexture.h"

#include "Labyrinth/Assets/AssetManager.h"
#include "Labyrinth/IO/Tiled.h"
#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

namespace Labyrinth {

	TilemapTexture::TilemapTexture(i32 width, i32 height, const std::unordered_map<TilePos, TileSpec>& behaviour)
		: mWidth(width), mHeight(height), mCamera(0.0f, 1.0f, 0.0f, 1.0f), mTileBehaviour(behaviour)
	{
		constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };

		FramebufferSpec fbSpec;
		fbSpec.width = mWidth * TileSize.x;
		fbSpec.height = mHeight * TileSize.y;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mTexture = Framebuffer::Create(fbSpec);

		mCamera = OrthographicCamera(0.0f, Cast<f32>(fbSpec.width), 0.0f, Cast<f32>(fbSpec.height));
	}

	TilemapTexture::TilemapTexture(const fs::path& path, const std::unordered_map<TilePos, TileSpec>& behaviour) :
		mCamera(0.0f, 1.0f, 0.0f, 1.0f), mTileBehaviour(behaviour)
	{
		TiledIO::Open(path, mLayers, mSheets);
		if (!mLayers.empty())
		{
			mWidth = mLayers[0].getWidth();
			mHeight = mLayers[0].getHeight();
		}

		constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };

		FramebufferSpec fbSpec;
		fbSpec.width = mWidth * TileSize.x;
		fbSpec.height = mHeight * TileSize.y;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mTexture = Framebuffer::Create(fbSpec);

		mCamera = OrthographicCamera(0.0f, Cast<f32>(fbSpec.width), 0.0f, Cast<f32>(fbSpec.height));
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

	void TilemapTexture::RegenTexture(bool overlay)
	{
		// TODO: maybe too large, estimate 100x100 map would be ~150MB of texture memory.
		constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };
		constexpr glm::vec2 TileSizeF = glm::vec2{ 64.0f };
		constexpr glm::vec4 ColliderColour = { 0, 1, 0, 1 };

		mTexture->bind();

		RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1 });
		RenderCommand::Clear();
		RenderCommand::DisableDepth();

		Renderer2D::BeginState(mCamera);

		for (const TexMapLayer& layer : mLayers)
		{
			for (usize y = 0; y < mHeight; y++)
			{
				for (usize x = 0; x < mWidth; x++)
				{
					auto tileData = layer[{x, mHeight - y - 1}];
					if (tileData.id == -1) continue;

					Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(GetSheet(tileData.id));
					glm::vec2 pos = { x * TileSize.x, y * TileSize.y };
					pos += 0.5f * TileSizeF;

					Renderer2D::DrawRotatedQuad(pos, TileSizeF, (f32)tileData.rotation, sheet->getSubTex(tileData.id));
				}
			}
		}

		if (overlay)
		{
			for (const auto& [pos, spec] : mTileBehaviour)
			{
				if (spec.solid)
				{
					glm::vec2 posF = { pos.x * TileSize.x, (mHeight - pos.y - 1) * TileSize.y };
					posF += 0.5f * TileSizeF;
					Renderer2D::DrawRect(posF, TileSizeF, ColliderColour);
				}
			}
		}

		Renderer2D::EndState();
		RenderCommand::EnableDepth();
		mTexture->unbind();
	}

	TileData TilemapTexture::getTile(usize layer, const TilePos& pos) const
	{
		if (layer >= mLayers.size())
			return TileData();

		return mLayers[layer][pos];
	}

	void TilemapTexture::setTile(usize layer, TilePos pos, TileData data)
	{
		if (layer >= mLayers.size())
			return;

		mLayers[layer][pos] = data;
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