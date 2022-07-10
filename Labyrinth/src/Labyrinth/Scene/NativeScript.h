#pragma once

#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	class NativeScript
	{
	public:
		virtual ~NativeScript() {}

		template<typename T>
		T& getComponent()
		{
			return mEntity.getComponent<T>();
		}

		bool isComplete() const { return mComplete; }

	protected:
		virtual void onStart() {}
		virtual void onStop() {}
		virtual void onUpdate(Timestep ts) {}

		bool mComplete = true;

	private:
		Entity mScriptEntity;
		friend class Scene;
	};
}