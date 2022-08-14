#include "Lpch.h"
#include "Tilemap.h"

#include "TilemapComponents.h"

#include "Labyrinth/IO/Tiled.h"
#include <Labyrinth/Renderer/Framebuffer.h>
#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

namespace Labyrinth {

	Tilemap::Tilemap(const std::string& name, usize width, usize height)
		: mMapName(name), mTexture(TilemapTexture::Create(width, height)), mWidth(width), mHeight(height)
	{
		RegenTexture();
		RegenEntities();
	}

	Tilemap::Tilemap(const fs::path& path, usize width, usize height)
		: mMapName(path.stem().string()), mTexture(TilemapTexture::Create(path, width, height)), mWidth(width), mHeight(height)
	{
		RegenTexture();
		RegenEntities();
	}

	void Tilemap::RegenEntities()
	{
		mMapEntity.removeChildren();
		mContext->getEntitiesWith<TileComponent>().each([&](auto entity, auto& component) { mContext->DestroyEntity({ entity, mContext }); });
		mEntityMap.clear();

		for (usize y = 0; y < mHeight; y++)
		{
			for (usize x = 0; x < mWidth; x++)
			{
				Entity entity = mContext->CreateEntity(fmt::format("{0}-({1},{2})", mMapName, x, y));
				entity.addComponent<ScriptComponent>();
				TileComponent& tc = entity.addComponent<TileComponent>(x, y);
				entity.setParent(mMapEntity);

				mEntityMap.emplace(tc.tile, entity);
			}
		}
	}
}