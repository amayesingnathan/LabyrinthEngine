project "Labyrinth-ScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"
		
    targetdir 	("%{wks.location}/Enigma/Resources/Scripts")
    objdir 		("%{wks.location}/Enigma/Resources/Scripts/Intermediates")

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