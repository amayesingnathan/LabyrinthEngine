#pragma once
#include "System.h"

class ScriptEngine : public System
{
public:
	void update() override;
	void clean() override;
};