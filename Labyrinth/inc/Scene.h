#pragma once

//Systems
#include "ECS/Systems/PhysicsEngine.h"
#include "ECS/Systems/InputManager.h"
#include "ECS/Systems/TextureManager.h"
#include "ECS/Systems/MapSystem.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/AssetManager.h"
#include "ECS/Systems/ScriptEngine.h"

class Scene
{
public:
	void init(int lvl);
	void update();
	void render();
	void clean();

public:
	//Systems
	static PhysicsEngine sysPhysics;
	static InputManager sysInput;
	static TextureManager sysTex;
	static Map sysMap;
	static Collision sysCollisions;
	static RenderSystem sysRender;
	static AssetManager sysAssets;
	static ScriptEngine sysScripting;

	static SDL_Rect camera;

	entt::registry mRegistry;

private:
	class Entity* player;
};

