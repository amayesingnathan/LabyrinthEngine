#include "Lpch.h"

#include "./GameComponents.h"

#include "ECS/Entity/Entity.h"

KeyboardController::KeyboardController(Entity* entt) :
	Component(entt, Types::Controller){}

