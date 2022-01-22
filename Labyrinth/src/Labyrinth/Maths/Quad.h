#pragma once

#include <cmath>

struct SDL_Rect;

namespace Labyrinth {

	struct TransformComponent;

	struct Quad
	{
		float x, y;
		float w, h;

		Quad() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) {}
		Quad(float _x, float _y, float _w, float _h)
			: x(_x), y(_y), w(_w), h(_h) {}

		Quad(int _x, int _y, int _w, int _h)
			: x((float)_x), y((float)_y), w((float)_w), h((float)_h) {}

		Quad(double _x, double _y, double _w, double _h)
			: x((float)_x), y((float)_y), w((float)_w), h((float)_h) {}

		Quad(const SDL_Rect& rect);

		operator SDL_Rect()&;
	};
}