#pragma once

#include "Entity.h"

namespace Labyrinth {

	class ScriptableEntity
	{
	public:
		ScriptableEntity() = default;

		template<typename T>
		T& getComponent()
		{
			return mEntity.getComponent<T>();
		}

	protected:
		virtual void onCreate() {}
		virtual void onDestroy() {}
		virtual void onUpdate(Timestep ts) {}

	private:
		Entity mEntity;
		friend class Scene;
	};
}