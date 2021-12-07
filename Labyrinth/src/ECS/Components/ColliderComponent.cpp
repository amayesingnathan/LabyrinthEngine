#include "ECS/Components/ColliderComponent.h"

#include "ECS/Components/TagComponent.h"
#include "ECS/Components/TransformComponent.h"

ColliderComponent::ColliderComponent(Entity& entt) :
	Component(entt), collider()
{
	tag = &entity.getComponent<TagComponent>();

	if (entity.hasComponent<TransformComponent>())
	{
		auto& transform = entity.getComponent<TransformComponent>();
		collider.x = static_cast<int>(transform.pos.x);
		collider.y = static_cast<int>(transform.pos.y);
		collider.w = transform.width * transform.scale;
		collider.h = transform.height * transform.scale;
	}
}	