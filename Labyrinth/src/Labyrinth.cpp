#include "Labyrinth.h"

#include "Scene.h"

#include "ECS/Entity/Entity.h"

SDL_Renderer* Labyrinth::renderer = nullptr;
SDL_Event Labyrinth::event;

const Uint8* Labyrinth::prevKeyboard;
const Uint8* Labyrinth::keyboard;

bool Labyrinth::isRunning;

int configuration::FPS;
float configuration::frameDelay;
int configuration::mFrameDelay;

Labyrinth::Labyrinth() :
	window(NULL),
	currScene(NULL)
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

		currScene->init(player, 1);
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
	if (!currScene) return;

	currScene->update();
}

void Labyrinth::render()
{
	if (!currScene) return;

	SDL_RenderClear(Labyrinth::renderer);
	currScene->render();
	SDL_RenderPresent(Labyrinth::renderer);

}

void Labyrinth::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(Labyrinth::renderer);
	SDL_Quit();
	std::cout << "Game Cleaned." << std::endl;
}
