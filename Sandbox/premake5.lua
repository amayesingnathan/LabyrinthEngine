project "Sandbox"
	kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
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
		"%{IncludeDir.asio}",
		"%{IncludeDir.entt}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui_cpp}",
        "%{IncludeDir.json}",
        "%{IncludeDir.spdlog}",
		"%{IncludeDir.yaml_cpp}",
    }
	
    links
    {
        "Labyrinth",
        "imgui-cpp",
    }

    filter "system:windows"
        systemversion "latest"

    filter "system:linux"
        links { "pthread", "dl" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"