#include "Lpch.h"

#include "ECS/Components/GameComponents.h"

#include "ECS/Entity/Entity.h"

KeyboardController::KeyboardController(Entity* entt) :
	Component(entt, Types::Controller){}

