#include "ECS/Systems/TextureManager.h"

#include "Labyrinth.h"
#include "Scene.h"

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
	auto sprites = registry->view<SpriteComponent>();

	for (auto sprite : sprites)
	{
		//Get components for physics from entity
		auto& draw = sprites.get<SpriteComponent>(sprite);

		//If the velocity is less than zero then sprite should be flipped.
		if (registry->all_of<VelocityComponent>(sprite))
		{
			auto& velocity = registry->get<VelocityComponent>(sprite);
			draw.spriteFlip = (velocity.vel.x >= 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		}

		//Update sprite animation if necessary
		if (draw.animated)
		{
			if (registry->all_of<VelocityComponent>(sprite))
			{
				play(draw, SpriteComponent::suppAnimations::Idle);

				auto& velocity = registry->get<VelocityComponent>(sprite);
				if (velocity.vel.x != 0)
				{
					play(draw, SpriteComponent::suppAnimations::Running);
				}
				draw.srcRect.x = draw.srcRect.w * static_cast<int>((SDL_GetTicks() / draw.speed) % draw.frames);
				draw.srcRect.y = draw.srcRect.h * static_cast<int>(draw.currAnimation);
			}
		}
	}
}

void TextureManager::play(SpriteComponent& sprite, const SpriteComponent::suppAnimations& anim)
{
	if (!sprite.animations.count(anim)) return; //Return if animation has not been added

	sprite.currAnimation = anim;
	sprite.frames = sprite.animations[anim].frames;
	sprite.speed = sprite.animations[anim].speed;
}
