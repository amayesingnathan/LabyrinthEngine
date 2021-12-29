#include "Lpch.h"

#include "./RenderSystem.h"

#include "Labyrinth.h"
#include "Scene.h"

void RenderSystem::update()
{
	//Use updated transform to update sprite position
	auto sprites = mScene->mRegistry.view<SpriteComponent, TransformComponent>();

	std::for_each(std::execution::par, sprites.begin(), sprites.end(), [&sprites](const auto entity) {
		//Get components for sprite from entity
		auto& sprite = sprites.get<SpriteComponent>(entity);
		const auto& transform = sprites.get<TransformComponent>(entity);

		sprite.destRect.x = static_cast<int>(transform.pos.x) - Scene::camera.x;
		sprite.destRect.y = static_cast<int>(transform.pos.y) - Scene::camera.y;
		sprite.destRect.w = static_cast<int>(round(transform.width * transform.scale.x));
		sprite.destRect.h = static_cast<int>(round(transform.height * transform.scale.y));
	});
}

void RenderSystem::render()
{
	//Get render layer to draw next
	for (auto& renderLayer : Scene::sysMap.renderLayers)
	{
		for (auto tile : renderLayer)
		{
			//Only draw tiles that are on screen.
			if ((tile->destRect.x + tile->destRect.w > 0) &&
				(tile->destRect.y + tile->destRect.h > 0) &&
				(tile->destRect.x - tile->destRect.w < Scene::camera.x + configuration::SCREEN_WIDTH) &&
				(tile->destRect.y - tile->destRect.h < Scene::camera.y + configuration::SCREEN_HEIGHT)
				)
			{
				draw(tile->sprite->texture, &tile->sprite->srcRect, &tile->destRect, tile->sprite->spriteFlip);
			}
		}
	}

	//Get entities with sprites but not tiles
	auto sprites = mScene->mRegistry.view<SpriteComponent>(entt::exclude<TileComponent>);
	for (auto entity : sprites)
	{
		auto& sprite = mScene->mRegistry.get<SpriteComponent>(entity);
		if (sprite.texture) draw(sprite.texture, &sprite.srcRect, &sprite.destRect, sprite.spriteFlip);
	}
}

void RenderSystem::draw(SDL_Texture* tex, const SDL_Rect* src, const SDL_Rect* dest, SDL_RendererFlip flip)
{
	if (tex == NULL) return;
	SDL_RenderCopyEx(Labyrinth::renderer, tex, src, dest, NULL, NULL, flip);
}