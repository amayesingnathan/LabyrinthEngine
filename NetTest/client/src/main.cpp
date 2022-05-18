#define LABYRINTH_APP_NAME "CLIENT"

#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "TestClient.h"

namespace Labyrinth {

	class LabyrinthClient : public Application
	{
	public:
		LabyrinthClient()
		{
			pushLayer(new Net::TestClientLayer());
		}

		~LabyrinthClient()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs)
	{
		return new LabyrinthClient();
	}
}