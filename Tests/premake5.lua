project "Tests"
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
        "%{IncludeDir.gtest}",
		"%{IncludeDir.imgui_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.json}",
		"%{IncludeDir.magic_enum}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.yaml_cpp}",
    }
	
    links
    {
        "Labyrinth",
        "gtest",
        "imgui-cpp",
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "system:linux"
        links { "pthread", "dl", "yaml-cpp" }
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        runtime "Release"
        optimize "on"