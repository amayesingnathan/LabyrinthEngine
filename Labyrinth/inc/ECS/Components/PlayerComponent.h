#pragma once

#include "ECS/Components/Component.h"

#include "SDL.h"

#include <string>

struct PlayerComponent : public Component
{
	PlayerComponent(Entity& entt, const SDL_Rect& rect, int sc);
};