#pragma once

#ifdef LAB_PLATFORM_WINDOWS

extern Labyrinth::Application* Labyrinth::CreateApplication();

int main(int argc, char** argv)
{	
	Labyrinth::Log::Init();

	LAB_PROFILE_BEGIN_SESSION("Startup", "LabyrinthProfile-Startup.json");
	auto app = Labyrinth::CreateApplication();	
	LAB_PROFILE_END_SESSION();

	LAB_PROFILE_BEGIN_SESSION("Runtime", "LabyrinthProfile-Runtime.json");
	app->run();
	LAB_PROFILE_END_SESSION();

	LAB_PROFILE_BEGIN_SESSION("Runtime", "LabyrinthProfile-Shutdown.json");
	delete app;
	LAB_PROFILE_END_SESSION();
}

#else
	#error Labyrinth only supports Windows!
#endif // LAB_PLATFORM_WINDOWS