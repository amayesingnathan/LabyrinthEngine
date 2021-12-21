#include "ECS/Systems/RenderSystem.h"

#include "Labyrinth.h"
#include "Scene.h"

void RenderSystem::render()
{
	//Draw map background first
	draw(Scene::sysMap.getBG(), NULL, NULL, SDL_FLIP_NONE);

	//Get tile map components to draw next
	auto tiles = registry->view<TileComponent>();
	for (auto entity : tiles)
	{
		auto& tile = registry->get<SpriteComponent>(entity);
		draw(tile.texture, &tile.srcRect, &tile.destRect, tile.spriteFlip);
	}

	//Get entities with sprites
	auto sprites = registry->view<SpriteComponent>();
	for (auto entity : sprites)
	{
		//Only draw sprites for entities that dont have a tile component because these were drawn already
		if (!registry->all_of<TileComponent>(entity))
		{
			auto& sprite = registry->get<SpriteComponent>(entity);
			if (sprite.texture) draw(sprite.texture, &sprite.srcRect, &sprite.destRect, sprite.spriteFlip);
		}
	}
}

void RenderSystem::draw(SDL_Texture* tex, const SDL_Rect* src, const SDL_Rect* dest, SDL_RendererFlip flip)
{
	if (tex == NULL) return;
	SDL_RenderCopyEx(Labyrinth::renderer, tex, src, dest, NULL, NULL, flip);
}
