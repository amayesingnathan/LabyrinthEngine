project "Sandbox"
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
        "%{IncludeDir.json}",
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

    filter "configurations:*d"
        runtime "Debug"
        symbols "on"

    filter "configurations:not *d"
        runtime "Release"
        optimize "on"