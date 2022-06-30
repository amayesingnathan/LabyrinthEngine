#include <Labyrinth.h>

#define LAB_APP_NAME "ENIGMA"
#include "Labyrinth/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Labyrinth {

	class EnigmaEditor : public Application
	{
	public:
		EnigmaEditor()
			: Application("Enigma")
		{
			pushLayer(new EditorLayer());
		}

		~EnigmaEditor()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EnigmaEditor();
	}

}
