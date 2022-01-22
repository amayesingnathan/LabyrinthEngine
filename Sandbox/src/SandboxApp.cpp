#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "Sandbox2D.h"

class Sandbox : public Labyrinth::Application
{
public:
	Sandbox() 
	{
		//pushLayer(new Labyrinth::ExampleLayer());
		pushLayer(new Sandbox2D());
	}

	~Sandbox() 
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication(ApplicationCommandLineArgs args)
{
	return new Sandbox();
}