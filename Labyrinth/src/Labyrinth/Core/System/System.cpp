#include "Lpch.h"
#include "System.h"

#include "Assert.h"
#include "Log.h"
#include "Platform.h"

namespace Laby {

	static void FormatPath(std::string& path);
	static std::string_view GetScriptExtension();

	void System::SetEnv(std::string_view envVar)
	{
#ifdef LAB_PLATFORM_WINDOWS
		int error = _putenv(envVar.data());
#elif defined(LAB_PLATFORM_LINUX)
		int error = putenv(envVar.data());
#else
		int error = -1;
#endif
		if (error)
			LAB_CORE_ERROR("Could not set the Labyrinth root directory!");
	}

	void System::Call(std::string_view cmd)
	{
		std::string cmdString(cmd);
		FormatPath(cmdString);
		system(cmdString.c_str());
	}



	void FormatPath(std::string& path)
	{
#ifdef LAB_PLATFORM_WINDOWS
		std::replace(path.begin(), path.end(), '/', '\\');
#endif
		path = fmt::format("{}{}", path, GetScriptExtension());
	}

	std::string_view GetScriptExtension()
	{
#ifdef LAB_PLATFORM_WINDOWS
		return ".bat";
#elif defined(LAB_PLATFORM_LINUX)
		return ".sh";
#else
		LAB_STATIC_ASSERT("Cannot get script extension on unsupported platform!");
		return "";
#endif
	}
}