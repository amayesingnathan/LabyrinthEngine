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
		//draw(tile.texture, &tile.srcRect, &tile.destRect, tile.spriteFlip);
	}

	//Get entities with textures
	auto sprites = registry->view<SpriteComponent>();
	for (auto entity : sprites)
	{
		//Only draw sprites for entities that dont have a tile component because this was drawn already
		if (!registry->all_of<TileComponent>(entity))
		{
			auto& sprite = registry->get<SpriteComponent>(entity);
			draw(sprite.texture, &sprite.srcRect, &sprite.destRect, sprite.spriteFlip);
		}
	}
}

void RenderSystem::update()
{
	auto sprites = registry->view<SpriteComponent, TransformComponent>();

	for (auto sprite : sprites)
	{
		//Get components for sprite from entity
		auto& draw = sprites.get<SpriteComponent>(sprite);
		auto& transform = sprites.get<TransformComponent>(sprite);
		draw.destRect.x = static_cast<int>(transform.pos.x);
		draw.destRect.y = static_cast<int>(transform.pos.y);
		draw.destRect.w = transform.width * transform.scale;
		draw.destRect.h = transform.height * transform.scale;
	}
}

void RenderSystem::draw(SDL_Texture* tex, const SDL_Rect* src, const SDL_Rect* dest, SDL_RendererFlip flip)
{
	if (tex == NULL) return;
	SDL_RenderCopyEx(Labyrinth::renderer, tex, src, dest, NULL, NULL, flip);
}
