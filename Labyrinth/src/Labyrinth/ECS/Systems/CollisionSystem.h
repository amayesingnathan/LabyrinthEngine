#pragma once

#if 0

#include "System.h"

#include "ECS/Components/Vector2D.h"

class Collision : public System
{
public:
	void init(class Scene* scene, class Entity* entt);
	void update() override;

	void addTrigger(class Entity& entity, void(*func)());

private:
	bool AABB(const SDL_Rect& recA, const SDL_Rect& recB, SDL_Rect* result);
	bool AABB(const struct ColliderComponent& colA, const struct ColliderComponent& colB, SDL_Rect* result);

	bool resolveCollision(struct TransformComponent& trans, bool hasVel);

private:
	class Entity* player;
	Vector2D bounds;
};

#endif