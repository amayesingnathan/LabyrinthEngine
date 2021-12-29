#include "Lpch.h"

#if 0

#include "Scene.h"

#include "ECS/Entity/ScriptableEntity.h"
#include "ECS/Components/GameComponents.h"

PhysicsEngine Scene::sysPhysics;
InputManager Scene::sysInput;
TextureManager Scene::sysTex;
Map Scene::sysMap;
Collision Scene::sysCollisions;
RenderSystem Scene::sysRender;
AssetManager Scene::sysAssets;
ScriptEngine Scene::sysScripting;

SDL_Rect Scene::camera;

void Scene::init()
{
	camera = {};

	//Initialise asset manager to create player entity. Required to pass to other systems.
	sysAssets.init(this);

	//Add player entity to the scene
	player = sysAssets.addPlayer();

	sysInput.init(this);
	sysPhysics.init(this);
	sysTex.init(this);
	sysCollisions.init(this, player);
	sysMap.init(this, player);
	sysRender.init(this);
	sysScripting.init(this);

	sysMap.loadLevel(1, { 390, 325 });

	//player->addComponent<ScriptComponent>().bind<TestScript>();
}

void Scene::update()
{
	//Scripting Engine call to handle all scripted entities
	sysScripting.update();

	//Input Manager call to handle all keyboard & mouse components
	sysInput.update();

	//Physics Engine call to handle all physics related components
	sysPhysics.update();

	//Map System call to update tile components relative to player
	sysMap.update();

	//Texture Manager call to handle all animations and textures
	sysTex.update();

	//Render System call to handle updating locations of sprites
	sysRender.update();

	//Collision System call to handle collisions with the player
	sysCollisions.update();
}

void Scene::render()
{
	sysRender.render();
}

void Scene::clean()
{
	sysInput.clean();
	sysPhysics.clean();
	sysTex.clean();
	sysCollisions.clean();
	sysMap.clean();
	sysRender.clean();
	sysScripting.clean();
}

#endif