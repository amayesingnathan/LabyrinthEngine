#include "Lpch.h"
#include "Tilemap.h"
#include "TiledIO.h"

#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

namespace Labyrinth {

	Tilemap::Tilemap(const std::string& name)
		: mMapName(name)
	{
		std::filesystem::path mapPath = "assets/tilemaps/" + name;

		TiledIO::Open(mapPath, mLayers, mSheets);

		uint32_t width = (uint32_t)mLayers[0].getWidth();
		uint32_t height = (uint32_t)mLayers[0].getHeight();
		glm::vec2 tileSize = mSheets[0].sheet->getTileSize();

		FramebufferSpec fbSpec;
		fbSpec.width = width * Cast<uint32_t>(tileSize.x);
		fbSpec.height = height * Cast<uint32_t>(tileSize.y);
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mTexture = Framebuffer::Create(fbSpec);
		genTex();
	}

	const Ref<Framebuffer>& Tilemap::genTex()
	{
		mTexture->bind();

		RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1 });
		RenderCommand::Clear();

		Renderer2D::BeginState();

		for (MapLayer& layer : mLayers)
		{
			for (size_t y = 0; y < layer.getHeight(); y++)
			{
				for (size_t x = 0; x < layer.getWidth(); x++)
				{
					size_t tileID = layer(x, y);
					if (tileID == 0) continue;

					Ref<Texture2DSheet> sheet = GetSheet(tileID);
					Renderer2D::DrawQuad({ x, y }, sheet->getTileSizeN(), sheet->getSubTex(std::to_string(tileID)));
				}
			}
		}

		Renderer2D::EndState();

		mTexture->unbind();
		
		return mTexture;
	}

	Ref<Tilemap> Tilemap::Create(const std::string& name)
	{
		return CreateRef<Tilemap>(name);
	}

	const Ref<Texture2DSheet>& Tilemap::GetSheet(size_t tileID)
	{
		auto it = std::find_if(mSheets.rbegin(), mSheets.rend(), [tileID](const SheetData& sheetData) 
		{
			return sheetData.firstID <= tileID;
		});

		if (it != mSheets.rend())
			return it->sheet;

		else return mSheets[0].sheet;
	}
}