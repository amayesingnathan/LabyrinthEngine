#include "Labyrinth.h"

#include "ECS/Entity.h"

SDL_Renderer* Labyrinth::renderer = nullptr;
SDL_Event Labyrinth::event;

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
	//Define refresh rate
	configuration::FPS = 60;
	configuration::frameDelay = static_cast<float>(1) / static_cast<float>(configuration::FPS);
	configuration::mFrameDelay = 1000 / configuration::FPS;

	//The Player entity. This should be initialised by your game. WASD support is added by default.
	player = CreateEntity("Player");

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
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}

		isRunning = true;
	}
	else {
		isRunning = false;
	}

}

void Labyrinth::handleEvents()
{
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	}
}

void Labyrinth::update()
{
	//Get entities that have Keyboard control
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
}

void Labyrinth::render()
{
	SDL_RenderClear(renderer);

	//Get entities with textures
	auto sprites = m_Registry.view<SpriteComponent>();
	for (auto entity : sprites)
	{
		auto& sprite = m_Registry.get<SpriteComponent>(entity);
		TextureManager::Draw(sprite.texture, sprite.srcRect, sprite.destRect);
	}

	SDL_RenderPresent(renderer);

}

Entity Labyrinth::CreateEntity(const std::string tag)
{
	Entity player = { m_Registry.create(), &m_Registry };
	player.addComponent<TagComponent>(player, tag);
	auto& phys = player.addComponent<PhysicsComponent>(player, 0.0f, false);
	auto& vel = player.addComponent<VelocityComponent>(player, 0.0f, &phys);
	player.addComponent<TransformComponent>(player, 0.0f, &vel);
	return player;
}

void Labyrinth::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game Cleaned." << std::endl;
}
