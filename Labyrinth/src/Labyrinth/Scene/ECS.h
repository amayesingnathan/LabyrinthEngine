#pragma once

#include "entt.hpp"

namespace Laby {

	using EntityID = entt::entity;
	using Registry = entt::registry;

	static constexpr auto NullEntity = entt::null;
}