#pragma once		

#include "ECS/Systems/System.h"

#include "ECS/Components/SpriteComponent.h"

#include "ECS/Entity/entt.hpp"

class TextureManager : public System
{
public:
	~TextureManager();

	SDL_Texture* loadTexture(const char* fileName);
	void destroyTexture(SDL_Texture* tex);

	void update();

	SDL_Rect setSubTex(int x = 0, int y = 0, int w = 32, int h = 32)
	{
		return { x, y, w, h };
	}

	static void play(SpriteComponent& sprite, SpriteComponent::suppAnimations anim);
};

