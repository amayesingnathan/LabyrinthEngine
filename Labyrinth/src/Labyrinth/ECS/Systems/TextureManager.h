#pragma once		

#include "System.h"

class TextureManager : public System
{
public:
	void update() override;
	void clean() override;

	SDL_Texture* loadTexture(const char* fileName);
	void destroyTexture(SDL_Texture* tex);

	SDL_Rect setSubTex(int x = 0, int y = 0, int w = 32, int h = 32)
	{
		return { x, y, w, h };
	}

	static void play(struct SpriteComponent& sprite, const std::string& anim);
};

