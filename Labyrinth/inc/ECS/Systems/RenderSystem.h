#pragma once

#include "ECS/Systems/System.h"

#include "SDL.h"

#include "ECS/Entity/entt.hpp"

class RenderSystem : public System
{
public:
	void update();
	void render();

private:
	void draw(SDL_Texture* tex, const SDL_Rect* src, const SDL_Rect* dest, SDL_RendererFlip flip);
};