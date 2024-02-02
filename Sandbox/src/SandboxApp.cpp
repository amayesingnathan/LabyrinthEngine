#define LAB_APP_NAME Sandbox

#include "Labyrinth/Core/EntryPoint.h"

#include "SandboxLayer.h"

namespace Laby {

	class SandboxApp : public Application
	{
	public:
		SandboxApp(ApplicationSpec* spec)
			: Application(spec)
		{
			PushLayer<SandboxLayer>();
		}

		~SandboxApp()
		{
		}
	};

}

slc::Application* CreateApplication(int argc, char** argv)
{
	Laby::ApplicationSpec* spec = new Laby::ApplicationSpec;
	spec->name = "Sandbox";
	spec->fullscreen = false;
	spec->resolution = { 1600, 900 };
	spec->workingDir = "../Enigma";

	return new Laby::SandboxApp(spec);
}
