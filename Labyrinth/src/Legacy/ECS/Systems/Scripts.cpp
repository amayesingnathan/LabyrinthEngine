#include "Lpch.h"

#if 0

#include "ECS/Entity/ScriptableEntity.h"

class TestScript : public ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
		std::cout << "TestScript::OnCreate" << std::endl;
	}

	virtual void OnDestroy() override
	{
		std::cout << "TestScript::OnDestroy" << std::endl;
	}

	virtual void OnUpdate() override
	{
		std::cout << "TestScript::OnUpdate" << std::endl;
	}
};

#endif