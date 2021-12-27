#pragma once

//ECS Components
#include "ECS/Components/GameComponents.h"

class Labyrinth
{
public: //Public enums
	enum eGravity { GravityON = true, GravityOFF = false };

public:
	Labyrinth();
	~Labyrinth();

	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	void handleEvents();
	void update();
	void render();
	void clean();


	static bool running() { return isRunning; };

	static SDL_Renderer* renderer;

	static SDL_Event event;

	static const Uint8* prevKeyboard;
	static const Uint8* keyboard;

	static bool isRunning;

protected:
	SDL_Window* window;
	std::unique_ptr<class Scene> currScene;

};

