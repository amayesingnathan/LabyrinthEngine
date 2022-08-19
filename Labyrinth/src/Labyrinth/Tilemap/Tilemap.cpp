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

}