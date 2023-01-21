#define LAB_APP_NAME Enigma

#include "Labyrinth/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Laby {

	class EnigmaEditor : public Application
	{
	public:
		EnigmaEditor(const ApplicationSpec& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer);
		}

		~EnigmaEditor()
		{
		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpec spec;
		spec.name = "Enigma";

		Application::ReadSettings("enigma.json", spec);

		return new EnigmaEditor(spec);
	}

}
