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
IncludeDir["glew"] = "Labyrinth/dependencies/glew/include"
IncludeDir["entt"] = "Labyrinth/dependencies/entt/include"
IncludeDir["spdlog"] = "Labyrinth/dependencies/spdlog/include"
IncludeDir["rapidxml"] = "Labyrinth/dependencies/rapidxml"

LibDir = {}
LibDir["SDL2"] = "Labyrinth/dependencies/SDL2/lib"
LibDir["glew"] = "Labyrinth/dependencies/glew/lib"

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
        "%{prj.name}/src/**.cpp"
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
        "%{IncludeDir.glew}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.rapidxml}"
    }
	
	libdirs
	{
		"%{LibDir.SDL2}",
		"%{LibDir.glew}"
	}

	links
	{
		"SDL2.lib",
		"glew32s",
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
        "Labyrinth/dependencies/spdlog/include",
        "Labyrinth/src"
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