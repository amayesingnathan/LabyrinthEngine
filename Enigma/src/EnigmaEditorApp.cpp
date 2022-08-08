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

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpec spec;
		spec.name = "Enigma";

		Application::ReadSettings("enigma.ini", spec);

		return new EnigmaEditor(spec);
	}

}
