#pragma once		

#include "ECS/Systems/System.h"

#include "ECS/Components/SpriteComponent.h"

class TextureManager : public System
{
public:
	SDL_Texture* loadTexture(const char* fileName);
	void destroyTexture(SDL_Texture* tex);

	void update();

	SDL_Rect setSubTex(int x = 0, int y = 0, int w = 32, int h = 32)
	{
		return { x, y, w, h };
	}

	void play(SpriteComponent& sprite, const SpriteComponent::suppAnimations& anim);
};

