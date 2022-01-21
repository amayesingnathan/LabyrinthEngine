project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
	
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")
	
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
		"%{IncludeDir.lua}",
        "%{IncludeDir.asio}"
    }
	
    links
    {
        "Labyrinth"
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines { "LAB_DEBUG" }
        symbols "on"

    filter "configurations:Release"
        defines { "LAB_RELEASE" }
        optimize "on"

    filter "configurations:Dist"
        defines { "LAB_DIST" }
		runtime "Release"
        optimize "on"