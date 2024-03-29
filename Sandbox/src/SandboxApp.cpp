#define LAB_APP_NAME Sandbox

#include "Labyrinth/Core/EntryPoint.h"

#include "SandboxLayer.h"

namespace Laby {

	class SandboxApp : public Application
	{
	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec)
		{
			PushLayer<SandboxLayer>();
		}

		~SandboxApp()
		{
		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpec spec;
		spec.name = "Sandbox";
		spec.fullscreen = false;
		spec.resolution = { 1600, 900 };
		spec.workingDir = "../Enigma";

		return new SandboxApp(spec);
	}

}
