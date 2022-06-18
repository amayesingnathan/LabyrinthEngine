include "./dependencies/premake/premake_customization/solution_items.lua"
include "dependencies.lua"

workspace "Labyrinth"
    startproject "Enigma"

    configurations { 
        "x64d",
        "x64", 
        "ARMd",
        "ARM",
        "ARM64d",
        "ARM64"
    }

	filter "configurations:x64d"
		architecture "x86_64"

 	filter "configurations:x64"
		architecture "x86_64"

	filter "configurations:ARMd"
		architecture "ARM"

 	filter "configurations:ARM"
		architecture "ARM"

	filter "configurations:ARM64d"
		architecture "ARM64"

 	filter "configurations:ARM64"
		architecture "ARM64"
	
	solution_items
	{
		".editorconfig"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "dependencies/premake"
	include "Labyrinth/dependencies/asio"
	include "Labyrinth/dependencies/box2d"
	include "Labyrinth/dependencies/glfw"
	include "Labyrinth/dependencies/glad"
	include "Labyrinth/dependencies/imgui"
	include "Labyrinth/dependencies/lua"
	include "Labyrinth/dependencies/yaml-cpp"
group ""

include "Labyrinth"
include "Sandbox"
include "Enigma"