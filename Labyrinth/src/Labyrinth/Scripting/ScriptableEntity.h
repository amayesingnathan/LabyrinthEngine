#pragma once

#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	class ScriptableEntity
	{
	public:
		template<typename T>
		T& getComponent()
		{
			return mEntity.getComponent<T>();
		}

		virtual void onNativeScript(NativeScriptComponent& nsc)
		{
			for (auto& script : nsc.runScripts)
			{
				script();
			}
		}

	private:
		Entity mEntity;
		friend class Scene;
	};
}