include "./dependencies/premake/premake_customization/solution_items.lua"
include "dependencies.lua"

workspace "Labyrinth"
    startproject "Enigma"

    configurations 
    { 
        "Debug",
        "Release"
    }
    
    platforms
    {
        "x64",
        "ARM32",
        "ARM64"
    }

	filter "platforms:x64"
		architecture "x86_64"

	filter "platforms:ARM32"
		architecture "ARM"

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
	include "Labyrinth/dependencies/mContainers"
	include "Labyrinth/dependencies/yaml-cpp"
group ""

group "Core"
	include "Labyrinth"
	include "Labyrinth-ScriptCore"
group ""

group "Tools"
	include "Enigma"
group ""

group "Misc"
	include "Sandbox"
	include "Continuum"
group ""
