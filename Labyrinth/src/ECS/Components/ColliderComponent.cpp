#include "ECS/Components/ColliderComponent.h"

#include "ECS/Entity/Entity.h"
#include "ECS/Components/TransformComponent.h"

ColliderComponent::ColliderComponent(Entity* entt) :
	Component(entt), collider()
{
	if (entity->hasComponent<TransformComponent>())
	{
		auto& transform = entity->getComponent<TransformComponent>();
		collider.x = static_cast<int>(transform.pos.x);
		collider.y = static_cast<int>(transform.pos.y);
		collider.w = transform.width * transform.scale;
		collider.h = transform.height * transform.scale;
	}
	else { collider = { 0 }; }
}
ColliderComponent::ColliderComponent(Entity* entt, const SDL_Rect& rect) : Component(entt), collider(rect) {}
