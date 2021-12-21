#pragma once
#include "SDL.h"
#include "ECS/Systems/System.h"

#include "ECS/Components/Vector2D.h"

class Entity;

struct ColliderComponent;

class Collision : public System
{
public:
	void init(entt::registry& reg, const Entity& entt, const Vector2D& screen)
	{
		System::init(reg);
		player = entt;
		bounds = screen;
	}

	void update() override;

private:
	bool AABB(const SDL_Rect& recA, const SDL_Rect& recB, SDL_Rect* result);
	bool AABB(const ColliderComponent& colA, const ColliderComponent& colB, SDL_Rect* result);

private:
	Entity player;
	Vector2D bounds;
};