#pragma once		

#include "SDL.h"

struct TextureManager
{
	static SDL_Texture* LoadTexture(const char* fileName);
	static void DestroyTexture(SDL_Texture* tex);

	static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest);

};

