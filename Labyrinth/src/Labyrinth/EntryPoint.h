#pragma once

#ifdef LAB_PLATFORM_WINDOWS

extern Labyrinth::Application* Labyrinth::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Labyrinth::CreateApplication();
	app->run();
	delete app;
}

#else
	#error Labyrinth only supports Windows!
#endif // LAB_PLATFORM_WINDOWS