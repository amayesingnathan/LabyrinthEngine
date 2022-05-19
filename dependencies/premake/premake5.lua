project "Premake"
	kind "Utility"

    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)

	files
	{
		"%{wks.location}/**premake5.lua"
	}

	postbuildmessage "Regenerating project files with Premake5!"
	postbuildcommands
	{
		"%{prj.location}/bin/premake5 %{_ACTION} --file=\"%{wks.location}/premake5.lua\""
	}
	