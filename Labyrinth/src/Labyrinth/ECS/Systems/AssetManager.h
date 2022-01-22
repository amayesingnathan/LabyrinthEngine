#pragma once
#include "System.h"

#if 0

class Entity;

class AssetManager : public System
{
public:
	void clean() override;

	Entity* createEntity(const std::string& tag);
	void destroyEntity(Entity& entity);

	Entity* addPlayer();

	/*
		Add component functions use enum type in Component base class to dynamic cast to actual component.
		Cannot use function template as must be defined in header, which requires complete Entity type.
		Canoot include Entity.h as it includes Scene.h for its templates, which includes this header, leading to 
		a recursive include relation.
	*/
	void addComponent(Entity& entity, struct Component* comp);
	void addComponents(Entity& entity, std::vector<struct Component*> comps);

private:
	template<typename T>
	void OnComponentAdded(Entity entity, T& component);

private:
	std::vector<Entity*> mEntities;
};

#endif