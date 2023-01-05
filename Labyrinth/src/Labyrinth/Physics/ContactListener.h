#pragma once

#include <box2d/box2d.h>

namespace Laby {

	class ContactListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {}
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {}
	};
}