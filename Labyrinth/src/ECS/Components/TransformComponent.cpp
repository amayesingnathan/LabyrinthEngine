#include "Lpch.h"

#include "ECS/Components/GameComponents.h"

#include "ECS/Entity/Entity.h"

TransformComponent::TransformComponent(Entity* entt, const SDL_Rect& rect, Vector2D sc) :
	Component(entt, Types::Transform), pos{ rect.x, rect.y }, lastSafePos{ rect.x, rect.y }, width(rect.w), height(rect.h), scale(sc) {}
