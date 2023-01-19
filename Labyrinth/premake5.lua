project "Labyrinth"
    language "C++"
    cppdialect "C++20"
		
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
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{IncludeDir.Labyrinth}",
        "%{IncludeDir.LabCore}",
        "%{IncludeDir.asio}",
        "%{IncludeDir.box2d}",
		"%{IncludeDir.entt}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.json}",
        "%{IncludeDir.magic_enum}",
        "%{IncludeDir.mono}",
        "%{IncludeDir.pfd}",
        "%{IncludeDir.rapidxml}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.yaml_cpp}",
    }

	links
	{
		"asio",
		"box2d",
		"glad",
		"glfw",
		"ImGui",
		"yaml-cpp",
         "%{Library.mono}"
	}
	
	filter "files:dependencies/ImGuizmo/**.cpp"
		flags { "NoPCH" }
	
    filter "system:windows"
        kind "StaticLib"
        staticruntime "off"
        systemversion "latest"
        links 
        {
            "opengl32.lib",
            "%{Library.WinSock}",
            "%{Library.WinMM}",
            "%{Library.WinVersion}",
            "%{Library.BCrypt}",
        }
		
	filter "system:linux"
        kind "SharedLib"
        staticruntime "off"
        defines "LAB_SHARED"
        pic "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
		runtime "Debug"
        symbols "on"
        links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
		runtime "Release"
        optimize "on"
        links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}