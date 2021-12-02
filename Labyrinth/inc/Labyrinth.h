#pragma once

//SDL Includes
#include <SDL.h>
#include <SDL_image.h>

//ECS Components
#include "ECS/GameComponents.h"

//Map
#include "Map.h"

//Systems
#include "Collision.h"

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


	Entity CreateEntity(const std::string tag, int x = 0, int y = 0, int w = 32, int h = 32, int sc = 1);

	static bool running() { return isRunning; };

	static SDL_Renderer* renderer;

	static SDL_Event event;

	static const Uint8* prevKeyboard;
	static const Uint8* keyboard;

	static Map map;

	static bool isRunning;

protected:
	entt::registry m_Registry;
	SDL_Window* window;
	Entity player;
	Entity testWall;

};

