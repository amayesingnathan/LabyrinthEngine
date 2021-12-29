#pragma once

#if 0

#include "Entity.h"

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
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnUpdate() {}

private:
	Entity mEntity;
	friend class ScriptEngine;

};

#endif