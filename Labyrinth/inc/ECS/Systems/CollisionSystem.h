#pragma once
#include "SDL.h"
#include "ECS/Systems/System.h"

class Entity;

struct ColliderComponent;

class Collision : public System
{
public:
	void init(entt::registry& reg, const Entity& entt)
	{
		System::init(reg);
		player = entt;
	}

	void update() override;

private:
	bool AABB(const SDL_Rect& recA, const SDL_Rect& recB);

	bool AABB(const ColliderComponent& colA, const ColliderComponent& colB);

private:
	Entity player;
};