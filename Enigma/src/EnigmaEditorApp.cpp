#include <Labyrinth.h>
#include "Labyrinth/Common/EntryPoint.h"

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
