#pragma once		

class TextureManager
{
public:
	TextureManager() {};
	~TextureManager() {};

	static SDL_Texture* LoadTexture(const char* fileName)
	{
		SDL_Surface* tempSurface = IMG_Load(fileName);
		SDL_Texture* tex = SDL_CreateTextureFromSurface(Labyrinth::renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
		return tex;
	}

	static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest)
	{
		SDL_RenderCopy(Labyrinth::renderer, tex, &src, &dest);)
	}
};

