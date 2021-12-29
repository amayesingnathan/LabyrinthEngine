#include "Lpch.h"

#include "./GameComponents.h"

#include "ECS/Entity/Entity.h"

Component::Component(Entity* entt, Types type) : entity{ entt }, derived(type)
{}
