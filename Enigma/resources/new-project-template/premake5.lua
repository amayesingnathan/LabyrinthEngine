workspace "$PROJECT_NAME$"
	targetdir "build"
	startproject "$PROJECT_NAME$"
	
	configurations 
	{ 
		"Debug", 
		"Release",
		"Dist"
	}

group "Labyrinth"
project "Labyrinth-ScriptCore"
	location "%{wks.location}/Labyrinth-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/Enigma/resources/scripts")
	objdir ("%{wks.location}/Enigma/resources/scripts/intermediates")

	files
	{
		"%{wks.location}/Labyrinth-ScriptCore/Source/**.cs"
	}

group ""

project "$PROJECT_NAME$"
	location "assets/scripts"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{prj.location}/bin")
	objdir ("%{prj.location}/int")

	files 
	{
		"assets/scripts/src/**.cs"
    }
    
    links
    {
        "Labyrinth-ScriptCore"
    }