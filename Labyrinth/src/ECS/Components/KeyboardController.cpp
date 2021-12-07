#include "Labyrinth.h"
#include "ECS/Components/KeyboardController.h"
#include "ECS/Components/VelocityComponent.h"
#include "ECS/Components/SpriteComponent.h"
#include "ECS/Entity/Entity.h"

float Vector2D::lerpDur = 0.1f;

KeyboardController::KeyboardController() : Component(), lerpTime(0.0f) {}

KeyboardController::KeyboardController(Entity entt) :
	Component(entt), lerpTime(0.0f) {}

