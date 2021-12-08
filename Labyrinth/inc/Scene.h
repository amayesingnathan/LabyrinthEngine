#pragma once

#include "ECS/Entity/Entity.h"

//Systems
#include "ECS/Systems/PhysicsEngine.h"
#include "ECS/Systems/InputManager.h"
#include "ECS/Systems/TextureManager.h"
#include "ECS/Systems/MapSystem.h"
#include "ECS/Systems/CollisionSystem.h"

class Scene
{
public:
	void init(int lvl);

	void update();

	void render();

	Entity CreateEntity(const std::string tag);

public:
	static PhysicsEngine sysPhysics;
	static InputManager sysInput;
	static TextureManager sysTex;
	static Map sysMap;
	static Collision sysCollisions;

private:
	Entity player;
	entt::registry m_Registry;
};

