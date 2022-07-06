#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "Sandbox2D.h"

class Sandbox : public Labyrinth::Application
{
public:
	Sandbox(const Labyrinth::ApplicationSpec& spec)
		: Application(spec)
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
	ApplicationSpec spec;
	spec.name = "Sandbox";
	spec.workingDir = "../Enigma";
	spec.commandLineArgs = args;

	return new Sandbox(spec);
}