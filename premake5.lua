workspace "Labyrinth"
    architecture "x64"
    startproject "Sandbox"

    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["SDL2"] = "Labyrinth/dependencies/SDL2-2.0.16/include"
IncludeDir["SDL2image"] = "Labyrinth/dependencies/SDL2_image-2.0.5/include"
IncludeDir["entt"] = "Labyrinth/dependencies/entt/include"
IncludeDir["spdlog"] = "Labyrinth/dependencies/spdlog/include"
IncludeDir["rapidxml"] = "Labyrinth/dependencies/rapidxml"

LibDir = {}
LibDir["SDL2"] = "Labyrinth/dependencies/SDL2-2.0.16/lib"
LibDir["SDL2image"] = "Labyrinth/dependencies/SDL2_image-2.0.5/lib"

project "Labyrinth"
    location "Labyrinth"
    kind "SharedLib"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "Lpch.h"
	pchsource "Labyrinth/src/Lpch.cpp"

    files 
    { 
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.SDL2}",
        "%{IncludeDir.SDL2image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.rapidxml}"
    }
	
	libdirs
	{
		"%{LibDir.SDL2}",
		"%{LibDir.SDL2image}"
	}
	
	links
	{
		"SDL2",
		"SDL2_image"
	}

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "LAB_PLATFORM_WINDOWS",
            "LAB_BUILD_DLL",
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        optimize "On"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
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
        "Labyrinth/dependencies/spdlog/include",
        "Labyrinth/src"
    }

	libdirs
	{
		"%{LibDir.SDL2}",
		"%{LibDir.SDL2image}"
	}
	
    links
    {
        "Labyrinth",
		"SDL2",
		"SDL2_image"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        optimize "On"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
        optimize "On"