#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Labyrinth {

	class EnigmaEditor : public Labyrinth::Application
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

	Application* Labyrinth::CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EnigmaEditor();
	}

}
