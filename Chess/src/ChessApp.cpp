#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "ChessLayer.h"

namespace Labyrinth {

	class ChessApp : public Application
	{
	public:
		ChessApp()
			: Application("Chess")
		{
			pushLayer(new ChessLayer());
		}

		~ChessApp()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new ChessApp();
	}
}
