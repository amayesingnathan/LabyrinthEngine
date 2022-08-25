#include "Lpch.h"
#include "Tilemap.h"

#include "Labyrinth/IO/Tiled.h"
#include <Labyrinth/Renderer/Framebuffer.h>
#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

namespace Labyrinth {

	Tilemap::Tilemap(const std::string& name, i32 width, i32 height)
		: mMapName(name), mTexture(TilemapTexture::Create(width, height)), mWidth(width), mHeight(height)
	{
		RegenTexture();
	}

	Tilemap::Tilemap(const fs::path& path)
		: mMapName(path.stem().string()), mTexture(TilemapTexture::Create(path))
	{
		RegenTexture();
	}

	Ref<Tilemap> Tilemap::Clone(const Ref<Tilemap>& other)
	{
		Ref<Tilemap> copy = Tilemap::Create(other->mMapName, other->mWidth, other->mHeight);

		for (const auto& layer : other->mTexture->mLayers)
			copy->addLayer(layer);
		for (const auto& sheet : other->mTexture->mSheets)
			copy->addSheet(sheet.firstID, sheet.sheet);
		for (const auto& [pos, script] : other->mTileBehaviour)
			copy->setTileBehaviour(pos, script);

		copy->RegenTexture();
		copy->handle = other->handle;

		return copy;
	}

}