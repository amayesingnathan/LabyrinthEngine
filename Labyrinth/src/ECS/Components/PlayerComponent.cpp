#include "ECS/Components/PlayerComponent.h"

#include "ECS/Components/GameComponents.h"

PlayerComponent::PlayerComponent(Entity& entt, const SDL_Rect& rect, int sc) : Component(entt)
{
	entity.addComponent<PhysicsComponent>(entity, 0.0f, false);
	entity.addComponent<VelocityComponent>(entity, 0.0f);
	entity.addComponent<TransformComponent>(entity, rect, sc);
	entity.addComponent<KeyboardController>(entity);
	entity.addComponent<ColliderComponent>(entity);
	std::string playerSpritePath = "assets/PlayerSprite.png";
	entity.addComponent<SpriteComponent>(entity, playerSpritePath.c_str(), rect, true);
}
