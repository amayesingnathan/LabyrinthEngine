#include "ECS\Components\Component.h"

#include "ECS/Entity/Entity.h"

Component::Component(Entity* entt, Types type) : entity{ entt }, derived(type)
{}
