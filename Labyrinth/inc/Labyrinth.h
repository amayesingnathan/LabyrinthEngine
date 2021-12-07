#pragma once

//SDL Includes
#include <SDL.h>
#include <SDL_image.h>

//ECS Components
#include "ECS/Components/GameComponents.h"

//Systems
#include "ECS/Systems/PhysicsEngine.h"
#include "ECS/Systems/InputManager.h"
#include "ECS/Systems/TextureManager.h"
#include "ECS/Systems/MapSystem.h"
#include "ECS/Systems/CollisionSystem.h"

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
	Entity CreateEntity(const std::string tag, const SDL_Rect& rect, int sc = 1);

	static bool running() { return isRunning; };

	static SDL_Renderer* renderer;

	static SDL_Event event;

	static const Uint8* prevKeyboard;
	static const Uint8* keyboard;


	static PhysicsEngine sysPhysics;
	static InputManager sysInput;
	static TextureManager sysTex;
	static Map sysMap;
	static Collision sysCollisions;

	static bool isRunning;

protected:
	static entt::registry m_Registry;
	SDL_Window* window;
	Entity player;
	Entity testWall;

};

