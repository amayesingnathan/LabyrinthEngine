#include "Lpch.h"
#include "ContactListener.h"

#include <Labyrinth/Scene/Components.h>
#include <Labyrinth/Scripting/ScriptEngine.h>

namespace Laby {

	void ContactListener::BeginContact(b2Contact* contact)
	{
		Ref<Scene> scene = ScriptEngine::GetContext();

		if (!scene)
			return;

		Entity entA = scene->findEntity(*(UUID*)contact->GetFixtureA()->GetUserData().pointer);
		Entity entB = scene->findEntity(*(UUID*)contact->GetFixtureB()->GetUserData().pointer);

		auto onCollisionBegin = [](Entity entity, Entity other)
		{
			if (!entity.hasComponent<ScriptComponent>())
				return;

			const auto& sc = entity.getComponent<ScriptComponent>();
			if (!sc.instance)
				return;

			sc.instance->onCollisionBegin(other);
		};

		onCollisionBegin(entA, entB);
		onCollisionBegin(entB, entA);
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		Ref<Scene> scene = ScriptEngine::GetContext();

		if (!scene)
			return;

		Entity entA = scene->findEntity(*(UUID*)contact->GetFixtureA()->GetUserData().pointer);
		Entity entB = scene->findEntity(*(UUID*)contact->GetFixtureB()->GetUserData().pointer);

		auto onCollisionEnd = [](Entity entity, Entity other)
		{
			if (!entity.hasComponent<ScriptComponent>())
				return;

			const auto& sc = entity.getComponent<ScriptComponent>();
			if (!sc.instance)
				return;

			sc.instance->onCollisionEnd(other);
		};

		onCollisionEnd(entA, entB);
		onCollisionEnd(entB, entA);

	}
}