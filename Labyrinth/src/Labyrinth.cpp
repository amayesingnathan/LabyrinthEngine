#include "Labyrinth.h"

#include "ECS/Entity/Entity.h"

SDL_Renderer* Labyrinth::renderer = nullptr;
SDL_Event Labyrinth::event;

const Uint8* Labyrinth::prevKeyboard;
const Uint8* Labyrinth::keyboard;

entt::registry Labyrinth::m_Registry;


PhysicsEngine Labyrinth::sysPhysics;
InputManager Labyrinth::sysInput;
TextureManager Labyrinth::sysTex;
Map Labyrinth::sysMap;
Collision Labyrinth::sysCollisions;

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
	//Initialise systems to the registry they are used for
	sysInput.init(m_Registry);
	sysPhysics.init(m_Registry);
	sysTex.init(m_Registry);

	keyboard = SDL_GetKeyboardState(NULL);
	prevKeyboard = SDL_GetKeyboardState(NULL);

	//Define refresh rate
	configuration::FPS = 60;
	configuration::frameDelay = static_cast<float>(1) / static_cast<float>(configuration::FPS);
	configuration::mFrameDelay = 1000 / configuration::FPS;

	//The Player entity. This should be initialised by your game. WASD support is added by default.
	player = CreateEntity("Player");
	testWall = CreateEntity("Wall", { 250, 250, 32, 32 }, 2);

	//Initialise collision system once player has been created
	sysCollisions.init(m_Registry, player);

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

		//Initialise map system once renderer has been created
		sysMap.init(m_Registry);

		//Load level 1 map
		sysMap.loadLevel(1);

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
	//Input Manager call to handle all keyboard & mouse components
	sysInput.update();

	//Physics Engine call to handle all physics related components
	sysPhysics.update();

	//Texture Manager call to handle drawing all textures and sprites
	sysTex.update();

	//Collision System call to handle collisions with the player
	sysCollisions.update();

}

void Labyrinth::render()
{
	SDL_RenderClear(Labyrinth::renderer);
	sysTex.render();
	SDL_RenderPresent(Labyrinth::renderer);

}

Entity Labyrinth::CreateEntity(const std::string tag)
{
	Entity newEnt = { m_Registry.create(), &m_Registry };
	newEnt.addComponent<TagComponent>(newEnt, tag);
	return newEnt;
}

Entity Labyrinth::CreateEntity(const std::string tag, const SDL_Rect& rect, int sc)
{
	Entity newEnt = { m_Registry.create(), &m_Registry };
	newEnt.addComponent<TagComponent>(newEnt, tag);
	newEnt.addComponent<TransformComponent>(newEnt, rect, sc);
	return newEnt;
}

void Labyrinth::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(Labyrinth::renderer);
	SDL_Quit();
	std::cout << "Game Cleaned." << std::endl;
}
