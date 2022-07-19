#include "Lpch.h"
#include "Tilemap.h"

#include "Labyrinth/IO/Tiled.h"
#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

namespace Labyrinth {

	Tilemap::Tilemap(const std::string& name)
		: mMapName(name)
	{
		std::filesystem::path mapPath = "assets/tilemaps/" + name;

		TiledIO::Open(mapPath, mLayers, mSheets);

		size_t width = mLayers[0].getWidth();
		size_t height = mLayers[0].getHeight();
		glm::vec2 tileSize = mSheets[0].sheet->getTileSize();

		FramebufferSpec fbSpec;
		mWidth = fbSpec.width = width * Cast<size_t>(tileSize.x);
		mHeight = fbSpec.height = height * Cast<size_t>(tileSize.y);
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);

		GenTex();
	}

	void Tilemap::loadMap(const std::string& name)
	{
		std::filesystem::path mapPath = "assets/tilemaps/" + name;

		mLayers.clear();
		mSheets.clear();

		TiledIO::Open(mapPath, mLayers, mSheets);

		size_t width = mLayers[0].getWidth();
		size_t height = mLayers[0].getHeight();
		glm::vec2 tileSize = mSheets[0].sheet->getTileSize();

		mFramebuffer->resize(width, height);

		GenTex();
	}

	void Tilemap::GenTex()
	{
		mFramebuffer->bind();

		RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1 });
		RenderCommand::Clear();
		RenderCommand::DisableDepth();

		Renderer2D::BeginState();

		for (const MapLayer& layer : mLayers)
		{
			size_t width = layer.getWidth();
			size_t height = layer.getHeight();
			for (size_t y = 0; y < height; y++)
			{
				for (size_t x = 0; x < width; x++)
				{
					size_t tileID = layer(x, height - y - 1);
					if (tileID == 0) continue;

					Ref<Texture2DSheet> sheet = GetSheet(tileID);
					glm::vec2 tiletexSize = { 2.f / (float)width, 2.f / (float)height };
					glm::vec2 pos = { x * tiletexSize.x, y * tiletexSize.y };
					pos -= 1.f;

					Renderer2D::DrawQuad(pos, tiletexSize, sheet->getSubTex(std::to_string(tileID)));
				}
			}
		}

		Renderer2D::EndState();
		RenderCommand::EnableDepth();

		mTexture = Texture2D::Create(mWidth, mHeight);

		uint8_t* texData = new uint8_t[4 * mWidth * mHeight];
		mFramebuffer->readData(0, texData);
		mTexture->setData(texData, 4 * mWidth * mHeight);
		delete[] texData;

		mFramebuffer->unbind();
	}

	const Ref<Texture2DSheet>& Tilemap::GetSheet(size_t tileID) const
	{
		auto it = std::find_if(mSheets.rbegin(), mSheets.rend(), [tileID](const SheetData& sheetData)
			{
				return sheetData.firstID <= tileID;
			});

		if (it != mSheets.rend())
			return it->sheet;

		else return mSheets[0].sheet;
	}

	Ref<Tilemap> Tilemap::Create(const std::string& name)
	{
		return CreateRef<Tilemap>(name);
	}
}