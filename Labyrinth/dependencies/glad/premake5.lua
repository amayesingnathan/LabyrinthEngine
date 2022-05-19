project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)

    files 
    { 
        "include/glad/glad.h", 
        "include/KHR/khrplatform.h", 
        "src/glad.c"
    }
	
	includedirs
	{
		"include"
	}

    filter "system:windows"
        systemversion "latest"
      
    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"

    filter "configurations:Release"
		runtime "Release"
        optimize "on"