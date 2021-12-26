#pragma once

#include "ECS/Components/Component.h"
#include "SDL_rect.h"

struct ColliderComponent : public Component
{
	//THIS MUST MIRROR ENUM "Type" IN XMLParser.h
	enum class Type { Solid = 0, Trigger };

	SDL_Rect collider;
	Type type;
	void(*triggerFunc)();

	ColliderComponent& operator=(const ColliderComponent&) = default;

	ColliderComponent(class Entity* entt, ColliderComponent::Type = Type::Solid, void (*fcnPtr)() = nullptr);
	ColliderComponent(class Entity* entt, const SDL_Rect& rect, ColliderComponent::Type = Type::Solid, void (*fcnPtr)() = nullptr);
};