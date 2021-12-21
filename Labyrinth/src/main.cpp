#include "Labyrinth.h"

//Test commit
#include "config.h"

Labyrinth* game = nullptr;

int main(int argc, char* args[])
{

	Uint32 frameStart;
	int frameTime;

	game = new Labyrinth();

	game->init("Labyrinth Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 640, false);

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