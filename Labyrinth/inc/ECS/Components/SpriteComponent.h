#pragma once

#include "SDL.h"

#include "ECS/Components/Component.h"
#include "ECS/Components/Animation.h"

struct SpriteComponent : public Component
{
	enum class suppAnimations { None = -1, Idle = 0, Moving = 1 };

	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	bool animated;
	int frames;
	int speed;

	suppAnimations currAnimation;
	std::map<suppAnimations, Animation> animations;

	SDL_RendererFlip spriteFlip;

	SpriteComponent(Entity& entt, const char* path, const SDL_Rect& src, bool mAnimated = false);
	//SpriteComponent(Entity& entt, const char* path, const SDL_Rect& src, bool mAnimated = false);

	SpriteComponent(Entity& entt, SDL_Texture& tex, const SDL_Rect& src, bool mAnimated = false);

	~SpriteComponent() override;

	void setTex(const char* path);

	void setSubTex(int x, int y, int w, int h);

	void addAnimation(suppAnimations anim, int f, int s)
	{
		animations.emplace(anim, Animation(f, s));
	}


};