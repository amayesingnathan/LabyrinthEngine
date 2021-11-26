#pragma once
#include "ECS/Component.h"

#include <string>

struct TagComponent : public Component
{

	std::string tag;

	TagComponent() = default;
	TagComponent(Entity& entt, const std::string& tag) : Component(entt), tag(tag) {}
	TagComponent(const TagComponent&) = default;
};