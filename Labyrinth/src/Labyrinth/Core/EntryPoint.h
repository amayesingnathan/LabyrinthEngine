#pragma once

#ifndef LABYRINTH_APP_NAME
#define LABYRINTH_APP_NAME "APP"
#endif

extern Labyrinth::Application* Labyrinth::CreateApplication();

int main(int argc, char** argv)
{	
	Labyrinth::Log::Init(LABYRINTH_APP_NAME);

	LAB_PROFILE_BEGIN_SESSION("Startup", "LabyrinthProfile-Startup.json");
	auto app = Labyrinth::CreateApplication();	
	LAB_PROFILE_END_SESSION();

	LAB_PROFILE_BEGIN_SESSION("Runtime", "LabyrinthProfile-Runtime.json");
	app->run();
	LAB_PROFILE_END_SESSION();

	LAB_PROFILE_BEGIN_SESSION("Shutdown", "LabyrinthProfile-Shutdown.json");
	delete app;
	LAB_PROFILE_END_SESSION();
}
