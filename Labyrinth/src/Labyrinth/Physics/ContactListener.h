#pragma once

#include <Labyrinth/Core/UUID.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>

#include <box2d/box2d.h>

namespace Labyrinth {

	class ContactListener : public b2ContactListener
	{
	public:
		ContactListener(const Ref<Scene>& scene)
			: mContext(scene) {}

	public:
		void BeginContact(b2Contact* contact) override
		{
			Entity entA = mContext->findEntity(*(UUID*)contact->GetFixtureA()->GetUserData().pointer);
			Entity entB = mContext->findEntity(*(UUID*)contact->GetFixtureB()->GetUserData().pointer);

			f32 data = contact->GetTangentSpeed();
			Buffer bufferA(4);
			bufferA.append(&data, sizeof(f32));

			MessageBus::AddMessageType("ContactBegin");
			Message contactMessageA = entA.newMessage("ContactBegin", entB.getNodeID(), bufferA);
			entA.send(contactMessageA);

			Buffer bufferB(4);
			bufferB.append(&data, sizeof(f32));

			Message contactMessageB = entB.newMessage("ContactBegin", entA.getNodeID(), bufferB);
			entB.send(contactMessageB);
		}

		void EndContact(b2Contact* contact) override
		{
			Entity entA = mContext->findEntity(*(UUID*)contact->GetFixtureA()->GetUserData().pointer);
			Entity entB = mContext->findEntity(*(UUID*)contact->GetFixtureB()->GetUserData().pointer);

			MessageBus::AddMessageType("ContactEnd");
			Message contactMessageA = entA.newMessage("ContactEnd", entB.getNodeID());
			entA.send(contactMessageA);

			Message contactMessageB = entB.newMessage("ContactEnd", entA.getNodeID());
			entB.send(contactMessageB);

		}
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {}
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {}

	private:
		Ref<Scene> mContext;
	};
}