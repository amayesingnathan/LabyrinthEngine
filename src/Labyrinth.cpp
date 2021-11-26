#include "Labyrinth.h"

SDL_Renderer* Labyrinth::renderer = nullptr;

Labyrinth::Labyrinth() :
	isRunning(false),
	window(NULL),
	player(entt::null)
{}

Labyrinth::~Labyrinth()
{
}

//Initialisation of the game engine. This should be called from the init function of your game instance.
void Labyrinth::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	//The Player entity. This should be initialised by your game. WASD support is added by default.
	player = m_Registry.create();

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
	SDL_Event event;
	SDL_PollEvent(&event);
	Entity Player = Entity(player, this);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;

	case SDL_KEYDOWN:
	{
		const auto& key = event.key.keysym.sym;
		handleKeyEvent(key, true);
		break;
	}

	case SDL_KEYUP:	
	{
		const auto& key = event.key.keysym.sym;
		handleKeyEvent(key, false);
		break;
	}
	default:
		break;
	}
}

void Labyrinth::update()
{
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

}

void Labyrinth::render()
{
	SDL_RenderClear(renderer);
	//Add stuff to render

	//Get entities with textures
	auto sprites = m_Registry.view<SpriteComponent>();
	for (auto entity : sprites)
	{
		auto& sprite = m_Registry.get<SpriteComponent>(entity);
		TextureManager::Draw(sprite.texture, sprite.srcRect, sprite.destRect);
	}

	SDL_RenderPresent(renderer);

}

void Labyrinth::handleKeyEvent(const SDL_Keycode& key, bool pressed)
{
	//Wrap entity ID for easier component access
	Entity Player(player, this);
	auto& vel = Player.getComponent<VelocityComponent>();

	if (pressed)
	{
		switch (key)
		{
		case SDLK_w:
			vel.vel.y = -3;
			break;

		case SDLK_a:
			vel.vel.x = -3;
			break;

		case SDLK_s:
			vel.vel.y = 3;
			break;

		case SDLK_d:
			vel.vel.x = 3;
			break;

		default:
			break;
		}
	}
	else
	{
		switch (key)
		{
		case SDLK_w:
			if (vel.vel.y < 0) vel.vel.y = 0;
			break;

		case SDLK_a:
			if (vel.vel.x < 0) vel.vel.x = 0;
			break;

		case SDLK_s:
			if (vel.vel.y > 0) vel.vel.y = 0;
			break;

		case SDLK_d:
			if (vel.vel.x > 0) vel.vel.x = 0;
			break;

		default:
			break;
		}
	}
}

void Labyrinth::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game Cleaned." << std::endl;
}
