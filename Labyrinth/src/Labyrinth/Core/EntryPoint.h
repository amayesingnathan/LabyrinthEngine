#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/Application.h"

#ifndef LAB_APP_NAME
	#define LAB_APP_NAME "APP"
#endif

extern Labyrinth::Application* Labyrinth::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{	
	Labyrinth::Log::Init(LAB_APP_NAME);

	LAB_PROFILE_BEGIN_SESSION("Startup", "LabyrinthProfile-Startup.json");
	auto app = Labyrinth::CreateApplication({ argc, argv });
	LAB_PROFILE_END_SESSION();

	LAB_PROFILE_BEGIN_SESSION("Runtime", "LabyrinthProfile-Runtime.json");
	app->run();
	LAB_PROFILE_END_SESSION();

	LAB_PROFILE_BEGIN_SESSION("Shutdown", "LabyrinthProfile-Shutdown.json");
	delete app;
	LAB_PROFILE_END_SESSION();
}