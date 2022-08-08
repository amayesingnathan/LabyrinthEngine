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

Labyrinth::Application* Labyrinth::CreateApplication(int argc, char** argv)
{
	ApplicationSpec spec;
	spec.name = "Sandbox";

	Application::ReadSettings("sandbox.ini", spec);
	spec.workingDir = "../Enigma";

	return new Sandbox(spec);
}