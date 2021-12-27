#include "Lpch.h"

#include "ECS/Systems/TextureManager.h"

#include "Labyrinth.h"
#include "Scene.h"

TextureManager::~TextureManager()
{
	auto sprites = mScene->mRegistry.view<SpriteComponent>();

	for (auto entity : sprites)
	{
		auto& sprite = mScene->mRegistry.get<SpriteComponent>(entity);
		destroyTexture(sprite.texture);
	}
}

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
	auto sprites = mScene->mRegistry.view<SpriteComponent, VelocityComponent>();

	std::for_each(std::execution::par, sprites.begin(), sprites.end(), [&sprites](const auto entity) {
		//Get components for physics from entity
		auto& sprite = sprites.get<SpriteComponent>(entity);
		const auto& velocity = sprites.get<VelocityComponent>(entity);

		sprite.spriteFlip = (velocity.vel.x >= 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

		//Update sprite animation if necessary
		if (sprite.animated)
		{
			play(sprite, "Idle");

			if (!velocity.vel.isNull())
			{
				play(sprite, "Running");
			}
			sprite.srcRect.x = sprite.srcRect.w * static_cast<int>((SDL_GetTicks() / sprite.speed) % sprite.frames);
			sprite.srcRect.y = sprite.srcRect.h * sprite.currAnimation.index;
		}
		});
}

void TextureManager::play(SpriteComponent& sprite, const std::string& anim)
{
	if (!sprite.animations.count(anim)) return; //Return if animation has not been added

	sprite.currAnimation = sprite.animations[anim];
	sprite.frames = sprite.animations[anim].frames;
	sprite.speed = sprite.animations[anim].speed;
}
