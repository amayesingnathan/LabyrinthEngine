#pragma once		

#include "SDL.h"

#include "ECS/Systems/System.h"

#include "ECS/Components/SpriteComponent.h"

class TextureManager : public System
{
public:
	SDL_Texture* loadTexture(const char* fileName);
	void destroyTexture(SDL_Texture* tex);

	void render();
	void draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip);

	void update();

	SDL_Rect setSubTex(int x = 0, int y = 0, int w = 32, int h = 32)
	{
		return { x, y, w, h };
	}

	void play(SpriteComponent& sprite, const SpriteComponent::suppAnimations& anim);
};

