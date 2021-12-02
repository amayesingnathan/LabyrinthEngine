#pragma once		

#include "SDL.h"

struct TextureManager
{
	static SDL_Texture* LoadTexture(const char* fileName);
	static void DestroyTexture(SDL_Texture* tex);

	static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest);

	static SDL_Rect setSubTex(int x = 0, int y = 0, int w = 32, int h = 32)
	{
		return { x, y, w, h };
	}
};

