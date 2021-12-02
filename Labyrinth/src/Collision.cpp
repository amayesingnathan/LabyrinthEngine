#include "Collision.h"

bool Collision::AABB(const SDL_Rect& recA, const SDL_Rect& recB)
{
	return SDL_HasIntersection(&recA, &recB);
}


bool Collision::AABB(const ColliderComponent* colA, const ColliderComponent* colB)
{
	return AABB(colA->collider, colB->collider);
}