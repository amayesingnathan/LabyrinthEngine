#pragma once
#include "ECS/Systems/System.h"

#include <vector>
#include <string>

class AssetManager : public System
{
public:
	~AssetManager();

	class Entity* createEntity(const std::string& tag);
	class Entity* addPlayer();

private:
	std::vector<class Entity*> mEntities;
};

