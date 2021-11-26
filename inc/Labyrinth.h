#pragma once

//SDL Includes
#include <SDL.h>
#include <SDL_image.h>

//ECS Components
#include "ECS/GameComponents.h"


//Standard Library Includes
#include <iostream>

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

	void handleKeyEvent(const SDL_Keycode& keyPressed, bool pressed);

	bool running() { return isRunning; };

	static SDL_Renderer* renderer;

protected:
	entt::registry m_Registry;
	SDL_Window* window;
	entt::entity player;

private:
	bool isRunning;
};

