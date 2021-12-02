#include "Labyrinth.h"

#include "ECS/Entity.h"

SDL_Renderer* Labyrinth::renderer = nullptr;
SDL_Event Labyrinth::event;

const Uint8* Labyrinth::prevKeyboard;
const Uint8* Labyrinth::keyboard;

Map Labyrinth::map;

bool Labyrinth::isRunning;

int configuration::FPS;
float configuration::frameDelay;
int configuration::mFrameDelay;

Labyrinth::Labyrinth() :
	window(NULL)
{}

Labyrinth::~Labyrinth()
{
}

//Initialisation of the game engine. This should be called from the init function of your game instance.
void Labyrinth::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	keyboard = SDL_GetKeyboardState(NULL);
	prevKeyboard = SDL_GetKeyboardState(NULL);

	//Define refresh rate
	configuration::FPS = 60;
	configuration::frameDelay = static_cast<float>(1) / static_cast<float>(configuration::FPS);
	configuration::mFrameDelay = 1000 / configuration::FPS;

	//The Player entity. This should be initialised by your game. WASD support is added by default.
	player = CreateEntity("Player", 0, 0, 8, 14, 3);
	testWall = CreateEntity("Wall", 250, 250, 32, 32, 2);

	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Subsystems Initialised..." << std::endl;

		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window)
		{
			std::cout << "Window created..." << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			std::cout << "Renderer created..." << std::endl;
		}

		//Load map textures from file
		map.init();

		//Load level 1 map
		map.loadLevel(1);

		isRunning = true;
	}
	else {
		isRunning = false;
	}

}

void Labyrinth::handleEvents()
{
	SDL_PollEvent(&event);
	prevKeyboard = keyboard;
	keyboard = SDL_GetKeyboardState(NULL);

	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	}
}

void Labyrinth::update()
{

	//Get entities that have Keyboard control and handle their key events
	auto players = m_Registry.view<KeyboardController>();

	for (auto control : players)
	{
		auto& keyControl = players.get<KeyboardController>(control);
		keyControl.update();
	}

	//Get entities that have required components to update physics
	auto bodies = m_Registry.view<TransformComponent, VelocityComponent, PhysicsComponent>();

	for (auto body : bodies)
	{
		//Get components for physics from entity
		auto [position, velocity, physics] = bodies.get<TransformComponent, VelocityComponent, PhysicsComponent>(body);
		
		//Update physics
		physics.update();
		velocity.update();
		position.update();

	}

	auto sprites = m_Registry.view<SpriteComponent>();

	for (auto sprite : sprites)
	{
		//Get components for physics from entity
		auto& draw = sprites.get<SpriteComponent>(sprite);

		//Update sprite
		draw.update();
	}

	auto colliders = m_Registry.view<ColliderComponent>();

	for (auto collider : colliders)
	{
		//Get collider component from entity
		auto& box = colliders.get<ColliderComponent>(collider);

		//Update collider
		box.update();
	}

	if (Collision::AABB(&player.getComponent<ColliderComponent>(), &testWall.getComponent<ColliderComponent>()))
	{
		std::cout << player.getComponent<TagComponent>().tag << " hit " << testWall.getComponent<TagComponent>().tag << std::endl;
		auto& vel = player.getComponent<VelocityComponent>();
		vel.vel *= -1;
	}

}

void Labyrinth::render()
{
	SDL_RenderClear(Labyrinth::renderer);

	//Draw map
	map.drawMap();

	//Get entities with textures
	auto sprites = m_Registry.view<SpriteComponent>();
	for (auto entity : sprites)
	{
		auto& sprite = m_Registry.get<SpriteComponent>(entity);
		TextureManager::Draw(sprite.texture, sprite.srcRect, sprite.destRect);
	}

	SDL_RenderPresent(Labyrinth::renderer);

}

Entity Labyrinth::CreateEntity(const std::string tag, int x, int y, int w, int h, int sc)
{
	Entity newEnt = { m_Registry.create(), &m_Registry };
	newEnt.addComponent<TagComponent>(newEnt, tag);
	newEnt.addComponent<PhysicsComponent>(newEnt, 0.0f, false);
	newEnt.addComponent<VelocityComponent>(newEnt, 0.0f);
	newEnt.addComponent<TransformComponent>(newEnt, Vector2D(x, y), w, h, sc);
	return newEnt;
}

void Labyrinth::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(Labyrinth::renderer);
	SDL_Quit();
	std::cout << "Game Cleaned." << std::endl;
}
