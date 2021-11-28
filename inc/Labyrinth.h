#pragma once

//SDL Includes
#include <SDL.h>
#include <SDL_image.h>

//ECS Components
#include "ECS/GameComponents.h"

//Map
#include "Map.h"

//Standard Library Includes
#include <iostream>

class Entity;

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


	Entity CreateEntity(const std::string tag);

	static bool running() { return isRunning; };

	static SDL_Renderer* renderer;

	static SDL_Event event;

	static Map map;

	static bool isRunning;

protected:
	entt::registry m_Registry;
	SDL_Window* window;
	Entity player;

};

