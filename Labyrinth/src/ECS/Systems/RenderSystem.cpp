#include "ECS/Systems/RenderSystem.h"

#include "Labyrinth.h"
#include "Scene.h"
#include "config.h"

void RenderSystem::update()
{
	//Use updated transform to update sprite position
	auto sprites = registry->view<SpriteComponent, TransformComponent>();

	for (auto sprite : sprites)
	{
		//Get components for sprite from entity
		auto& draw = sprites.get<SpriteComponent>(sprite);
		const auto& transform = sprites.get<TransformComponent>(sprite);
		draw.destRect.x = static_cast<int>(transform.pos.x) - Scene::camera.x;
		draw.destRect.y = static_cast<int>(transform.pos.y) - Scene::camera.y;
		draw.destRect.w = transform.width * transform.scale;
		draw.destRect.h = transform.height * transform.scale;
	}
}

void RenderSystem::render()
{
	//Draw map background first
	draw(Scene::sysMap.getBG(), NULL, NULL, SDL_FLIP_NONE);

	//Get tile map components to draw next
	auto tiles = registry->view<TileComponent>();
	for (auto entity : tiles)
	{
		auto& tile = registry->get<TileComponent>(entity);
		auto& sprite = registry->get<SpriteComponent>(entity);
		if ((tile.destRect.x + tile.destRect.w > 0) &&
			(tile.destRect.y + tile.destRect.h > 0) &&
			(tile.destRect.x - tile.destRect.w < Scene::camera.x + configuration::SCREEN_WIDTH) &&
			(tile.destRect.y - tile.destRect.h < Scene::camera.y + configuration::SCREEN_HEIGHT)
			)
		{
			draw(sprite.texture, &sprite.srcRect, &tile.destRect, sprite.spriteFlip);
		}
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
