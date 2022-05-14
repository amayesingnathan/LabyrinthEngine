#include <Labyrinth/Core/EntryPoint.h>

#include "FluidLayer.h"

namespace Labyrinth {

	class Continuum : public Application
	{
	public:
		Continuum()
		{
			pushLayer(new FluidLayer());
		}

		~Continuum()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new Continuum();
	}
}
