#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

class EnigmaEditor : public Labyrinth::Application
{
public:
	EnigmaEditor()
	{
	}

	~EnigmaEditor()
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication()
{
	return new EnigmaEditor();
}