#pragma once

//#ifdef LAB_PLATFORM_WINDOWS

namespace Labyrinth{

	extern Application* CreateApplication();

	int main(int argc, char* argv[])
	{	
		Log::Init();

		LAB_PROFILE_BEGIN_SESSION("Startup", "LabyrinthProfile-Startup.json");
		auto app = CreateApplication();	
		LAB_PROFILE_END_SESSION();

		LAB_PROFILE_BEGIN_SESSION("Runtime", "LabyrinthProfile-Runtime.json");
		app->run();
		LAB_PROFILE_END_SESSION();

		LAB_PROFILE_BEGIN_SESSION("Shutdown", "LabyrinthProfile-Shutdown.json");
		delete app;
		LAB_PROFILE_END_SESSION();

		return 0;
	}

}


//#else
//#error Labyrinth only supports Windows!
//#endif // LAB_PLATFORM_WINDOWS