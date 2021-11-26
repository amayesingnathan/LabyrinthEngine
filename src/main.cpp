#include "Game.h"

Game* game = nullptr;

int main(int argc, char* args[])
{
	//Define refresh rate
	configuration::FPS = 60;
	configuration::frameDelay = static_cast<float>(1) / static_cast<float>(configuration::FPS);
	configuration::mFrameDelay = 1000 / configuration::FPS;

	Uint32 frameStart;
	int frameTime;

	game = new Game();

	game->init("Labyrinth Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);

	while (game->running())
	{
		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameTime < configuration::mFrameDelay)
		{
			SDL_Delay(configuration::mFrameDelay - frameTime);
		}
		else
		{
			if (configuration::mFrameDelay - frameTime > 0) {
				std::cout << "Running behind by " << (configuration::mFrameDelay - frameTime) << "ms." << std::endl;
			}
		}
	}

	game->clean();

	return 0;

}	