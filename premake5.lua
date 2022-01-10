workspace "Labyrinth"
    architecture "x86_64"
    startproject "Enigma"

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
IncludeDir["stb_image"] = "Labyrinth/dependencies/stb_image"
IncludeDir["glad"] = "Labyrinth/dependencies/glad/include"
IncludeDir["glm"] = "Labyrinth/dependencies/glm"
IncludeDir["ImGui"] = "Labyrinth/dependencies/imgui"
IncludeDir["entt"] = "Labyrinth/dependencies/entt/include"
IncludeDir["spdlog"] = "Labyrinth/dependencies/spdlog/include"
IncludeDir["rapidxml"] = "Labyrinth/dependencies/rapidxml"

LibDir = {}
LibDir["SDL2"] = "Labyrinth/dependencies/SDL2/lib"

group "Dependencies"
	include "Labyrinth/dependencies/glad"
	include "Labyrinth/dependencies/imgui"

group ""

project "Labyrinth"
    location "Labyrinth"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
		
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "Lpch.h"
	pchsource "Labyrinth/src/Lpch.cpp"

    files 
    { 
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/dependencies/stb_image/**.h",
        "%{prj.name}/dependencies/stb_image/**.cpp",
        "%{prj.name}/dependencies/glm/glm/**.hpp"
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.SDL2}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.rapidxml}"
    }
	
	libdirs
	{
		"%{LibDir.SDL2}",
	}

	links
	{
		"SDL2.lib",
		"glad",
		"ImGui",
		"opengl32.lib"
	}
	
    filter "system:windows"
        systemversion "latest"

        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
		runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
		runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
		runtime "Release"
        optimize "on"

project "Enigma"
    location "Enigma"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
	
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
        "%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
    }
	
    links
    {
        "Labyrinth"
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        symbols "on"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        optimize "on"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
		runtime "Release"
        optimize "on"
       
	   
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
	
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
        "%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
    }
	
    links
    {
        "Labyrinth"
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        symbols "on"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        optimize "on"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
		runtime "Release"
        optimize "on"