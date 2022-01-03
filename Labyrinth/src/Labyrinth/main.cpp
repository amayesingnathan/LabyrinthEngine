#include "Lpch.h"

#include "Labyrinth.h"

#if 0
constexpr int configuration::SCREEN_WIDTH;
constexpr int configuration::SCREEN_HEIGHT;

//Initialise game pointer to null
Labyrinth* game = nullptr;

int main(int argc, char* args[])
{
	Uint32 frameStart;
	int frameTime;

	game = new Labyrinth();

	game->init("Labyrinth Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, configuration::SCREEN_WIDTH, configuration::SCREEN_HEIGHT, false);
	S
	while (game->running())
	{
		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();

		frameTime = SDL_GetTicks() - frameStart;

		//std::cout << "Frame duration: " << frameTime << "ms.\n";

		if (frameTime < configuration::mFrameDelay)
		{
			SDL_Delay(configuration::mFrameDelay - frameTime);
		}
		else
		{
			std::cout << "Running behind by " << (frameTime - configuration::mFrameDelay) << "ms." << std::endl;
			
		}
	}

	game->clean();

	return 0;

}
#endif