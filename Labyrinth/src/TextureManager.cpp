#include "TextureManager.h"
#include "Labyrinth.h"

#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const char* fileName)
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

void TextureManager::DestroyTexture(SDL_Texture* tex)
{
	SDL_DestroyTexture(tex);
}

void TextureManager::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip)
{
	SDL_RenderCopyEx(Labyrinth::renderer, tex, &src, &dest, NULL, NULL, flip);
}