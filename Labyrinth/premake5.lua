project "Labyrinth"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
		
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "Lpch.h"
	pchsource "src/Lpch.cpp"

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
        "dependencies/stb_image/**.h",
        "dependencies/stb_image/**.cpp",
        "dependencies/glm/glm/**.hpp"
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "src",
        "%{IncludeDir.SDL2}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.rapidxml}",
        "%{IncludeDir.yaml_cpp}"
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
		"yaml-cpp",
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
