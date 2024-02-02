#define LAB_APP_NAME Enigma

#include "Labyrinth/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Laby {

	class EnigmaEditor : public Application
	{
	public:
		EnigmaEditor(ApplicationSpec* spec)
			: Application(spec)
		{
			PushLayer<EditorLayer>();
		}

		~EnigmaEditor()
		{
		}
	};


}

slc::Application* CreateApplication(int argc, char** argv)
{
	Laby::ApplicationSpec* spec = new Laby::ApplicationSpec;
	spec->name = "Enigma";

	Laby::Application::ReadSettings("enigma.json", *spec);

	return new Laby::EnigmaEditor(spec);
}
