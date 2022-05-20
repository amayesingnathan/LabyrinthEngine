project "Labyrinth"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
		
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

	links
	{
		"asio",
		"glad",
		"glfw",
		"ImGui",
		"yaml-cpp",
		"lua"
	}
	
	filter "files:dependencies/ImGuizmo/**.cpp"
		flags { "NoPCH" }
	
    filter "system:windows"
        systemversion "latest"
		includedirs "%{IncludeDir.LabWin}"
        links "opengl32.lib"
		
	filter "system:linux"
        pic "On"
        systemversion "latest"
        includedirs "%{IncludeDir.LabLinux}"

    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
		runtime "Debug"
        symbols "on"

	filter "configurations:ARMd"
        defines { "LAB_DEBUG" }
		runtime "Debug"
        symbols "on"

	filter "configurations:ARM64d"
        defines { "LAB_DEBUG" }
		runtime "Debug"
		symbols "on"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
		runtime "Release"
        optimize "on"

	filter "configurations:ARM"
        defines { "LAB_RELEASE" }
		runtime "Release"
		optimize "on"

	filter "configurations:ARM64"
        defines { "LAB_RELEASE" }
		runtime "Release"
		optimize "on"