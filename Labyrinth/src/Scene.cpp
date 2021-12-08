#include "Scene.h"

#include "ECS/Components/TagComponent.h"
#include "ECS/Components/PlayerComponent.h"

entt::registry Scene::m_Registry;

PhysicsEngine Scene::sysPhysics;
InputManager Scene::sysInput;
TextureManager Scene::sysTex;
Map Scene::sysMap;
Collision Scene::sysCollisions;

void Scene::init(Entity& player, int lvl)
{
	//The Player entity. This should be initialised by your game.
	player = CreateEntity("Player");

	//Initialise systems to this scene registry
	sysInput.init(m_Registry);
	sysPhysics.init(m_Registry);
	sysTex.init(m_Registry);
	sysCollisions.init(m_Registry, player);
	sysMap.init(m_Registry);

	//Load map for this scene
	sysMap.loadLevel(lvl);

	player.addComponent<PlayerComponent>(player, SDL_Rect{ 0, 0, 16, 22 }, 3);
}

void Scene::update()
{
	//Input Manager call to handle all keyboard & mouse components
	sysInput.update();

	//Physics Engine call to handle all physics related components
	sysPhysics.update();

	//Texture Manager call to handle drawing all textures and sprites
	sysTex.update();

	//Collision System call to handle collisions with the player
	sysCollisions.update();
}

void Scene::render()
{
}

Entity Scene::CreateEntity(const std::string tag)
{
	Entity newEnt = { m_Registry.create(), &m_Registry };
	newEnt.addComponent<TagComponent>(newEnt, tag);
	return newEnt;
}