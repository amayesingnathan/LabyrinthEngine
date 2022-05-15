project "Labyrinth"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)
	
	pchheader "Lpch.h"
	pchsource "src/Lpch.cpp"

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
        "dependencies/stb_image/**.h",
        "dependencies/stb_image/**.cpp",
        "dependencies/glm/glm/**.hpp",
        "dependencies/ImGuizmo/ImGuizmo.h",
        "dependencies/ImGuizmo/ImGuizmo.cpp",
        "dependencies/lua/**.hpp"
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{IncludeDir.Labyrinth}",
        "%{IncludeDir.LabCore}",
        "%{IncludeDir.RenderAPI}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.rapidxml}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.asio}"
    }
	
	libdirs
	{
		"%{LibDir.lua}"
	}

	links
	{
		"asio",
		"glad",
		"glfw",
		"ImGui",
		"yaml-cpp",
		"lua54",
		"opengl32.lib"
	}
	
	filter "files:dependencies/ImGuizmo/**.cpp"
		flags { "NoPCH" }
	
    filter "system:windows"
        systemversion "latest"
		includedirs "%{IncludeDir.LabWin}"
		
	filter "system:linux"
        systemversion "latest"
        includedirs "%{IncludeDir.LabLinux}"
        
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
