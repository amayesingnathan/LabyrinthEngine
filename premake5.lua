workspace "Labyrinth"
    architecture "x64"
    startproject "Sandbox"

    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["SDL2"] = "Labyrinth/dependencies/SDL2/include"
IncludeDir["glad"] = "Labyrinth/dependencies/glad/include"
IncludeDir["glm"] = "Labyrinth/dependencies/glm"
IncludeDir["ImGui"] = "Labyrinth/dependencies/imgui"
IncludeDir["entt"] = "Labyrinth/dependencies/entt/include"
IncludeDir["spdlog"] = "Labyrinth/dependencies/spdlog/include"
IncludeDir["rapidxml"] = "Labyrinth/dependencies/rapidxml"

LibDir = {}
LibDir["SDL2"] = "Labyrinth/dependencies/SDL2/lib"

include "Labyrinth/dependencies/glad"
include "Labyrinth/dependencies/imgui"

project "Labyrinth"
    location "Labyrinth"
    kind "StaticLib"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "Lpch.h"
	pchsource "Labyrinth/src/Lpch.cpp"

    files 
    { 
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/dependencies/glm/glm/**.hpp"
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.SDL2}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.rapidxml}"
    }
	
	libdirs
	{
		"%{LibDir.SDL2}"
	}

	links
	{
		"SDL2.lib",
		"glad",
		"ImGui",
		"opengl32.lib"
	}
	
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
			"GLEW_STATIC"
        }

		links
		{
			"SDL2main"
		}

        --postbuildcommands
        --{
        --    ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        --}
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        buildoptions "/MTd"
        symbols "On"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        buildoptions "/MT"
        optimize "On"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
        buildoptions "/MT"
        optimize "On"

        
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/int/" .. outputdir .. "/%{prj.name}")
	
    files 
    { 
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp" 
    }
    
    includedirs
    {
        "Labyrinth/src",
		"Labyrinth/dependencies",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}"
    }
	
    links
    {
        "Labyrinth"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        buildoptions "/MTd"
        symbols "On"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        buildoptions "/MT"
        optimize "On"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
        buildoptions "/MT"
        optimize "On"