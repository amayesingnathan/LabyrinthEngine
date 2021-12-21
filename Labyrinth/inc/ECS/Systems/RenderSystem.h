#pragma once

#include "ECS/Systems/System.h"

#include "SDL.h"

class RenderSystem : public System
{
public:
	void render();
	void update();

private:
	void draw(SDL_Texture* tex, const SDL_Rect* src, const SDL_Rect* dest, SDL_RendererFlip flip);
};