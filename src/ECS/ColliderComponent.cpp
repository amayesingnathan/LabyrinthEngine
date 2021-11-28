#include "ECS/ColliderComponent.h"

#include "ECS/TagComponent.h"
#include "ECS/TransformComponent.h"

ColliderComponent::ColliderComponent(Entity& entt, SDL_Rect rect) :
	Component(entt), collider(rect)
{
	tag = &entity.getComponent<TagComponent>();
	transform = &entity.getComponent<TransformComponent>();
}

void ColliderComponent::update()
{

}