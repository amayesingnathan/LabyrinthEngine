#include "Lpch.h"
#include "KeyEvent.h"

#include "SDL.h"

namespace Labyrinth {

	char KeyEvent::getKeyName() const
	{
		return (char)SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(mKeyCode));
	}

}
