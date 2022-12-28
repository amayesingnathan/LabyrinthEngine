#define LAB_APP_NAME Sandbox

#include "Labyrinth/Core/EntryPoint.h"

namespace Laby {

	class SandboxApp : public Application
	{
	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec)
		{
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

		return new SandboxApp(spec);
	}

}
