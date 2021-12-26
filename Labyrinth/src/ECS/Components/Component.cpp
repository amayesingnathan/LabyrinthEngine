#include "ECS\Components\Component.h"

#include "ECS/Entity/Entity.h"

Component::Component(Entity* entt) : entity{ entt }
{}
