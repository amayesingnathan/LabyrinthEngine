LAB_ROOT_DIR = os.getenv("LAB_ROOT_DIR")

workspace "SandboxProject"
	targetdir "build"
	startproject "SandboxProject"

    configurations {  
        "x64d",
        "x64", 
        "ARMd",
        "ARM",
        "ARM64d",
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

project "SandboxProject"
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