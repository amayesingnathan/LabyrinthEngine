#pragma once

#if 0

#include "System.h"

class RenderSystem : public System
{
public:
	void update() override;
	void render();

private:
	void draw(SDL_Texture* tex, const SDL_Rect* src, const SDL_Rect* dest, SDL_RendererFlip flip);
};

#endif