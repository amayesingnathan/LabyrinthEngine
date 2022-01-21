#define LABYRINTH_APP_NAME "CLIENT"

#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "LabyrinthClient.h"

namespace Labyrinth {

	class LabyrinthClient : public Labyrinth::Application
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

	Labyrinth::Application* Labyrinth::CreateApplication()
	{
		return new LabyrinthClient();
	}
}