#include "Scene.h"

#include "ECS/Entity/Entity.h"
#include "ECS/Components/GameComponents.h"

#include "config.h"

#include "SDL_rect.h"

PhysicsEngine Scene::sysPhysics;
InputManager Scene::sysInput;
TextureManager Scene::sysTex;
Map Scene::sysMap;
Collision Scene::sysCollisions;
RenderSystem Scene::sysRender;
AssetManager Scene::sysAssets;

SDL_Rect Scene::camera;

void Scene::init(int lvl)
{
	camera = {};

	//Initialise asset manager to create player entity. Required to pass to other systems.
	sysAssets.init(this);

	//Add player entity to the scene
	player = sysAssets.addPlayer();

	//Initialise systems to this scene
	sysInput.init(this);
	sysPhysics.init(this);
	sysTex.init(this);
	sysCollisions.init(this, player);
	sysMap.init(this, player);
	sysRender.init(this);

	//Load map for this scene
	sysMap.loadLevel(lvl);


}

void Scene::update()
{
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
