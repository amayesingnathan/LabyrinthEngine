#pragma once
#include "System.h"

class ScriptEngine : public System
{
public:
	void update() override;
	void clean() override;

	template<typename T> 
	void addScript(class Entity* entity);
};
