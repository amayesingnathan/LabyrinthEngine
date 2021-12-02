#include "ECS/ColliderComponent.h"

#include "ECS/TagComponent.h"
#include "ECS/TransformComponent.h"

ColliderComponent::ColliderComponent(Entity& entt) :
	Component(entt), collider()
{
	tag = &entity.getComponent<TagComponent>();
	transform = &entity.getComponent<TransformComponent>();

	collider.x = static_cast<int>(transform->pos.x);
	collider.y = static_cast<int>(transform->pos.y);
	collider.w = transform->width * transform->scale;
	collider.h = transform->height * transform->scale;
}	

void ColliderComponent::update()
{
	collider.x = static_cast<int>(transform->pos.x);
	collider.y = static_cast<int>(transform->pos.y);
	collider.w = transform->width * transform->scale;
	collider.h = transform->height * transform->scale;
}