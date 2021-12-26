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

	/*
		Add component functions use enum type in Component base class to dynamic cast to actual component.
		Cannot use function template as must be defined in header, which requires complete Entity type.
		Canoot include Entity.h as it includes Scene.h for its templates, which includes this header, leading to 
		a recursive include relation.
	*/
	void addComponent(class Entity& entity, struct Component* comp);
	void addComponents(class Entity& entity, std::vector<struct Component*> comps);

private:
	std::vector<class Entity*> mEntities;
};

