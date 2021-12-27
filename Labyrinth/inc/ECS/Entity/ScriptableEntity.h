#pragma once

#include "Entity.h"

class ScriptableEntity 
{
public:
	template<typename T>
	T& getComponent()
	{
		return mEntity.getComponent<T>();
	}

private:
	Entity mEntity;
	friend class Scene;

};