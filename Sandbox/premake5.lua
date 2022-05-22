project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
	
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
        links { "pthread", "dl" }

    filter "configurations:x64d"
        runtime "Debug"
        symbols "on"

    filter "configurations:ARMd"
        runtime "Debug"
        symbols "on"

    filter "configurations:ARM64d"
        runtime "Debug"
        symbols "on"

    filter "configurations:x64"
        runtime "Release"
        optimize "on"

    filter "configurations:ARM"
        runtime "Release"
        optimize "on"

    filter "configurations:ARM64"
        runtime "Release"
        optimize "on"