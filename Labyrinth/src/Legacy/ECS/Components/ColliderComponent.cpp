#include "Lpch.h"

#if 0

#include "./GameComponents.h"

#include "ECS/Entity/Entity.h"

ColliderComponent::ColliderComponent(Entity* entt, ColliderComponent::Type t, void (*fcnPtr)()) :
	Component(entt, Types::Collider), collider(), type(t)
{
	if (entity->hasComponent<TransformComponent>())
	{
		auto& transform = entity->getComponent<TransformComponent>();
		collider.x = static_cast<int>(transform.pos.x);
		collider.y = static_cast<int>(transform.pos.y);
		collider.w = static_cast<int>(round(transform.width * transform.scale.x));
		collider.h = static_cast<int>(round(transform.height * transform.scale.y));
	}
	else { collider = { 0 }; }

	switch (type)
	{
	case Type::Solid:
		triggerFunc = nullptr;
		break;

	case Type::Trigger:
		triggerFunc = fcnPtr;
		break;

	default:
		triggerFunc = nullptr;
		break;
	}
}

ColliderComponent::ColliderComponent(Entity* entt, const SDL_Rect& rect, ColliderComponent::Type t, void(*fcnPtr)()) : 
	Component(entt, Types::Collider), collider(rect), type(t)
{
	switch (type)
	{
	case Type::Solid:
		triggerFunc = nullptr;
		break;

	case Type::Trigger:
		triggerFunc = fcnPtr;
		break;

	default:
		triggerFunc = nullptr;
		break;
	}
}

#endif