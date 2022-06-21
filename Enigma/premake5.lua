project "Enigma"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
	
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)
	
    files 
    { 
        "src/**.h", 
        "src/**.cpp" 
    }
    
    includedirs
    {
        "%{IncludeDir.Labyrinth}",
        "%{IncludeDir.LabCore}",
		"%{wks.location}/Labyrinth/dependencies",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.lua}"
    }
	
    links
    {
        "Labyrinth"
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "system:linux"
        links { "pthread", "dl", "yaml-cpp" }
        
    filter "configurations:x64d"
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

    filter "configurations:x64"
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
