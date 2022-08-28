project "Labyrinth-ScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"
	namespace "Labyrinth"
		
    targetdir 	("%{wks.location}/Enigma/resources/scripts")
    objdir 		("%{wks.location}/Enigma/resources/scripts/intermediates")

    files 
    { 
        "Source/**.cs", 
        "Properties/**.cs", 
    }

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"

    filter "configurations:Release"
		runtime "Release"
        optimize "on"