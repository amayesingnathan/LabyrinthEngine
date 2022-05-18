#define LABYRINTH_APP_NAME "SERVER"

#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "TestServer.h"

namespace Labyrinth {

	class LabyrinthServer : public Application
	{
	public:
		LabyrinthServer()
		{
			pushLayer(new Net::TestServerLayer(60000));
		}

		~LabyrinthServer()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs)
	{
		return new LabyrinthServer();
	}
}