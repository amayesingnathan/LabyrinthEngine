include "./dependencies/premake/premake_customization/solution_items.lua"

workspace "Labyrinth"
    architecture "x86_64"
    startproject "Enigma"

    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }
	
	solution_items
	{
		".editorconfig"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Labyrinth"] = "%{wks.location}/Labyrinth/src"
IncludeDir["LabCore"] 	= "%{wks.location}/Labyrinth/src/Labyrinth/Core"
IncludeDir["RenderAPI"] = "%{wks.location}/Labyrinth/src/Labyrinth/Renderer/APIs"
IncludeDir["LabWin"] 	= "%{wks.location}/Labyrinth/src/Platforms/Windows"
IncludeDir["LabWin"] 	= "%{wks.location}/Labyrinth/src/Platforms/Linux"
IncludeDir["SDL2"] 		= "%{wks.location}/Labyrinth/dependencies/SDL2/include"
IncludeDir["stb_image"] = "%{wks.location}/Labyrinth/dependencies/stb_image"
IncludeDir["glad"] 		= "%{wks.location}/Labyrinth/dependencies/glad/include"
IncludeDir["glm"] 		= "%{wks.location}/Labyrinth/dependencies/glm"
IncludeDir["ImGui"] 	= "%{wks.location}/Labyrinth/dependencies/imgui"
IncludeDir["ImGuizmo"] 	= "%{wks.location}/Labyrinth/dependencies/ImGuizmo"
IncludeDir["entt"] 		= "%{wks.location}/Labyrinth/dependencies/EnTT/include"
IncludeDir["spdlog"] 	= "%{wks.location}/Labyrinth/dependencies/spdlog/include"
IncludeDir["rapidxml"] 	= "%{wks.location}/Labyrinth/dependencies/rapidxml"
IncludeDir["yaml_cpp"] 	= "%{wks.location}/Labyrinth/dependencies/yaml-cpp/include"
IncludeDir["lua"] 		= "%{wks.location}/Labyrinth/dependencies/lua/include"

LibDir = {}
LibDir["lua"] = "%{wks.location}/Labyrinth/dependencies/lua/lib"

group "Dependencies"
	include "dependencies/premake"
	include "Labyrinth/dependencies/glad"
	include "Labyrinth/dependencies/SDL2"
	include "Labyrinth/dependencies/imgui"
	include "Labyrinth/dependencies/yaml-cpp"
group ""

include "Labyrinth"
include "Sandbox"
include "Enigma"