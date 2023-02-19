#include "Lpch.h"
#include "Utils.h"

#include <box2d/b2_body.h>

namespace Laby {

	constexpr b2BodyType PhysicsUtils::BodyTypeToBox2D(RigidBodyComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case RigidBodyComponent::BodyType::Static: return b2_staticBody;
		case RigidBodyComponent::BodyType::Dynamic: return b2_dynamicBody;
		case RigidBodyComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		LAB_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}
}