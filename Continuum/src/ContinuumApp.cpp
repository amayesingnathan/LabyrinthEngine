#include <Labyrinth/Core/EntryPoint.h>

#include "FluidLayer.h"

namespace Labyrinth {

	class Continuum : public Application
	{
	public:
		Continuum(const ApplicationSpec& spec)
			: Application(spec)
		{
			pushLayer(new FluidLayer());
		}

		~Continuum()
		{
		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpec spec;
		spec.name = "Sandbox";

		Application::ReadSettings("sandbox.ini", spec);
		spec.workingDir = "../Enigma";

		return new Continuum(spec);
	}
}
