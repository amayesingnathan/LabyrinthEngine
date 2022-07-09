#include <Labyrinth.h>

#define LAB_APP_NAME Enigma
#include "Labyrinth/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Labyrinth {

	class EnigmaEditor : public Application
	{
	public:
		EnigmaEditor(const ApplicationSpec& spec)
			: Application(spec)
		{
			pushLayer(new EditorLayer());
		}

		~EnigmaEditor()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpec spec;
		spec.name = "Enigma";
		spec.commandLineArgs = args;

		JsonObj settings = JSON::Open("enigma.ini");
		if (settings.contains("Startup"))
			spec.fullscreen = settings["Startup"]["Fullscreen"];

		return new EnigmaEditor(spec);
	}

}
