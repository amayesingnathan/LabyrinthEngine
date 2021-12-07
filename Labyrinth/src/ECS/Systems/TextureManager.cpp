#include "ECS/Systems/TextureManager.h"
#include "Labyrinth.h"

#include "ECS/Components/GameComponents.h"

#include <iostream>

SDL_Texture* TextureManager::loadTexture(const char* fileName)
{
	SDL_Texture* tex = nullptr;
	SDL_Surface* tempSurface = IMG_Load(fileName);
	if (tempSurface == NULL)
	{
		std::cout << "Could not load textures." << std::endl;
	}
	else
	{
		tex = SDL_CreateTextureFromSurface(Labyrinth::renderer, tempSurface);
		if (tex == NULL)
		{
			std::cout << SDL_GetError() << std::endl;
		}
	}
	SDL_FreeSurface(tempSurface);
	return tex;
}

void TextureManager::destroyTexture(SDL_Texture* tex)
{
	SDL_DestroyTexture(tex);
}

void TextureManager::update()
{
	auto sprites = registry->view<SpriteComponent, TransformComponent>();

	for (auto sprite : sprites)
	{
		//Get components for physics from entity
		auto& draw = sprites.get<SpriteComponent>(sprite);

		//Update sprite
		if (draw.animated)
		{
			if (registry->all_of<VelocityComponent>(sprite))
			{
				auto& velocity = registry->get<VelocityComponent>(sprite);
				if (velocity.vel.isNull())
				{
					play(draw, SpriteComponent::suppAnimations::Idle);
				}
				else
				{
					play(draw, SpriteComponent::suppAnimations::Moving);
				}
				draw.srcRect.x = draw.srcRect.w * static_cast<int>((SDL_GetTicks() / draw.speed) % draw.frames);
				draw.srcRect.y = draw.srcRect.h * static_cast<int>(draw.currAnimation);
			}
		}

		auto& transform = sprites.get<TransformComponent>(sprite);
		draw.destRect.x = static_cast<int>(transform.pos.x);
		draw.destRect.y = static_cast<int>(transform.pos.y);
		draw.destRect.w = transform.width * transform.scale;
		draw.destRect.h = transform.height * transform.scale;
	}
}

void TextureManager::render()
{
	//Get entities with textures
	auto tiles = registry->view<TileComponent>();
	for (auto entity : tiles)
	{
		auto& tile = registry->get<SpriteComponent>(entity);
		draw(tile.texture, tile.srcRect, tile.destRect, tile.spriteFlip);
	}

	//Get entities with textures
	auto sprites = registry->view<SpriteComponent>();
	for (auto entity : sprites)
	{
		//Only draw sprites for entities that dont have a tile component because this was drawn already
		if (!registry->all_of<TileComponent>(entity))
		{
			auto& sprite = registry->get<SpriteComponent>(entity);
			draw(sprite.texture, sprite.srcRect, sprite.destRect, sprite.spriteFlip);
		}
	}
}

void TextureManager::draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip)
{
	SDL_RenderCopyEx(Labyrinth::renderer, tex, &src, &dest, NULL, NULL, flip);
}

void TextureManager::play(SpriteComponent& sprite, const SpriteComponent::suppAnimations& anim)
{
	if (!sprite.animations.count(anim)) return; //Return if animation has not been added

	sprite.currAnimation = anim;
	sprite.frames = sprite.animations[anim].frames;
	sprite.speed = sprite.animations[anim].speed;
}
