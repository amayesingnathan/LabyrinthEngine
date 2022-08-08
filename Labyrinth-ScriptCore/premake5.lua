project "Labyrinth-ScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"
		
    targetdir 	("%{wks.location}/Enigma/resources/scripts")
    objdir 		("%{wks.location}/Enigma/resources/scripts/intermediates")

    files 
    { 
        "Source/**.cs", 
        "Properties/**.cs", 
    }

    filter "configurations:*d"
		runtime "Debug"
        symbols "on"

    filter "configurations:not *d"
		runtime "Release"
        optimize "on"