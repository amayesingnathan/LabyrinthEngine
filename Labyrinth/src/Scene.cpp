#include "Scene.h"

#include "ECS/Components/GameComponents.h"

#include "config.h"

#include "SDL_rect.h"

PhysicsEngine Scene::sysPhysics;
InputManager Scene::sysInput;
TextureManager Scene::sysTex;
Map Scene::sysMap;
Collision Scene::sysCollisions;
RenderSystem Scene::sysRender;

SDL_Rect Scene::camera;

void Scene::init(int lvl)
{
	addPlayer();

	camera = {};

	//Initialise systems to this scene registry
	sysInput.init(m_Registry);
	sysPhysics.init(m_Registry);
	sysTex.init(m_Registry);
	sysCollisions.init(m_Registry, player, {800, 640});
	sysMap.init(m_Registry, player);
	sysRender.init(m_Registry);

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

Entity Scene::CreateEntity(const std::string tag)
{
	Entity newEnt = { m_Registry.create(), &m_Registry };
	newEnt.addComponent<TagComponent>(newEnt, tag);
	return newEnt;
}

void Scene::addPlayer()
{
	player = CreateEntity("player");

	SDL_Rect rect{ configuration::SCREEN_WIDTH / 2, configuration::SCREEN_HEIGHT / 2, 16, 22 };
	int scale = 2;

	player.addComponent<VelocityComponent>(player, 0.0f);
	player.addComponent<TransformComponent>(player, rect, scale);
	player.addComponent<KeyboardController>(player);
	player.addComponent<ColliderComponent>(player);
	std::string playerSpritePath = "assets/PlayerSprite.png";
	player.addComponent<SpriteComponent>(player, playerSpritePath.c_str(), rect, true);
}
