include "./dependencies/premake/premake_customization/solution_items.lua"

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

IncludeDir = {}
IncludeDir["Labyrinth"] = "%{wks.location}/Labyrinth/src"
IncludeDir["LabCore"] 	= "%{wks.location}/Labyrinth/src/Labyrinth/Core"
IncludeDir["LabWin"] 	= "%{wks.location}/Labyrinth/src/Platforms/Windows"
IncludeDir["LabLinux"] 	= "%{wks.location}/Labyrinth/src/Platforms/Linux"
IncludeDir["RenderAPI"] = "%{wks.location}/Labyrinth/src/Labyrinth/Renderer/APIs"
IncludeDir["asio"] 		= "%{wks.location}/Labyrinth/dependencies/asio/include"
IncludeDir["entt"] 		= "%{wks.location}/Labyrinth/dependencies/entt/include"
IncludeDir["glad"] 		= "%{wks.location}/Labyrinth/dependencies/glad/include"
IncludeDir["glfw"] 		= "%{wks.location}/Labyrinth/dependencies/glfw/include"
IncludeDir["glm"] 		= "%{wks.location}/Labyrinth/dependencies/glm"
IncludeDir["ImGui"] 	= "%{wks.location}/Labyrinth/dependencies/imgui"
IncludeDir["ImGuizmo"] 	= "%{wks.location}/Labyrinth/dependencies/ImGuizmo"
IncludeDir["lua"] 		= "%{wks.location}/Labyrinth/dependencies/lua"
IncludeDir["pfd"] 		= "%{wks.location}/Labyrinth/dependencies/portable-file-dialogs"
IncludeDir["rapidxml"] 	= "%{wks.location}/Labyrinth/dependencies/rapidxml"
IncludeDir["spdlog"] 	= "%{wks.location}/Labyrinth/dependencies/spdlog/include"
IncludeDir["stb_image"] = "%{wks.location}/Labyrinth/dependencies/stb_image"
IncludeDir["yaml_cpp"] 	= "%{wks.location}/Labyrinth/dependencies/yaml-cpp/include"

group "Dependencies"
	include "dependencies/premake"
	include "Labyrinth/dependencies/asio"
	include "Labyrinth/dependencies/glfw"
	include "Labyrinth/dependencies/glad"
	include "Labyrinth/dependencies/imgui"
	include "Labyrinth/dependencies/lua"
	include "Labyrinth/dependencies/yaml-cpp"
group ""

include "Labyrinth"
include "Sandbox"
include "Enigma"