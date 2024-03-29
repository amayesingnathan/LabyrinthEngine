LAB_ROOT_DIR = os.getenv("LAB_ROOT_DIR")

workspace "$PROJECT_NAME$"
	targetdir "build"
	startproject "$PROJECT_NAME$"

    configurations 
    { 
        "Debug",
        "Release"
    }
    
    platforms
    {
        "x64",
        "ARM32",
        "ARM64"
    }
	
group "Labyrinth"
project "Labyrinth-ScriptCore"
	location "%{LAB_ROOT_DIR}/Labyrinth-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{LAB_ROOT_DIR}/Enigma/resources/scripts")
	objdir ("%{LAB_ROOT_DIR}/Enigma/resources/scripts/intermediates")

	files
	{
		"%{LAB_ROOT_DIR}/Labyrinth-ScriptCore/Source/**.cs",
        "%{LAB_ROOT_DIR}/Labyrinth-ScriptCore/Properties/**.cs"
	}

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"

    filter "configurations:Release"
		runtime "Release"
        optimize "on"

group ""

project "$PROJECT_NAME$"
	location "assets/scripts"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{prj.location}/Binaries")
	objdir ("%{prj.location}/Intermediates")

	files 
	{
		"assets/scripts/Source/**.cs"
    }
    
    links
    {
        "Labyrinth-ScriptCore"
    }