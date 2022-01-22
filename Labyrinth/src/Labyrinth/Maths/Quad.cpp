#include "Lpch.h"
#include "Quad.h"

#include "SDL_rect.h"

namespace Labyrinth {

	Quad::Quad(const SDL_Rect& rect)
		: x((float)rect.x), y((float)rect.y), w((float)rect.w), h((float)rect.h) {}

	Quad::operator SDL_Rect()&
	{
		return SDL_Rect{ (int)(round(x)), (int)(round(y)), (int)(round(w)), (int)(round(h)) }; 
	}

}