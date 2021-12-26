#pragma once
#include "ECS/Systems/System.h"

#include "ECS/Components/Vector2D.h"

#include "SDL.h"

struct ColliderComponent;

class Collision : public System
{
public:
	void init(class Scene* scene, class Entity* entt);

	void update() override;

private:
	bool AABB(const SDL_Rect& recA, const SDL_Rect& recB, SDL_Rect* result);
	bool AABB(const ColliderComponent& colA, const ColliderComponent& colB, SDL_Rect* result);

private:
	class Entity* player;
	Vector2D bounds;
};