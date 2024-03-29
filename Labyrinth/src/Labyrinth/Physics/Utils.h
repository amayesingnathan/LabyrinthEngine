#pragma once

#include <Labyrinth/Scene/Components.h>

enum b2BodyType;

namespace Laby {

	class PhysicsUtils
	{
	public:
		static b2BodyType BodyTypeToBox2D(RigidBodyComponent::BodyType bodyType);
	};
}