#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/Application.h"

#ifndef LAB_APP_NAME
	#define LAB_APP_NAME App
#endif

#define LAB_APP_NAME_STR LAB_STRINGIFY_MACRO(LAB_APP_NAME)

int main(int argc, char** argv)
{
	Laby::Log::Init(LAB_APP_NAME_STR);

	Laby::Application::Run(argc, argv);
}