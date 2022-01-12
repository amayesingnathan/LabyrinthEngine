#include "Lpch.h"
#include "Labyrinth/Tools/PlatformUtils.h"

#include <commdlg.h>
#include "SDL.h"
#include "SDL_syswm.h"

#include "Labyrinth/Core/Application.h"

namespace Labyrinth {

	//Windows File Dialog Definition
#ifdef LAB_PLATFORM_WINDOWS

	std::optional<std::string> FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo((SDL_Window*)Application::Get().getWindow().getNativeWindow(), &wmInfo);
		ofn.hwndOwner = wmInfo.info.win.window;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		SDL_CaptureMouse(SDL_TRUE);
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			SDL_CaptureMouse(SDL_FALSE);
			return ofn.lpstrFile;
		}
		SDL_CaptureMouse(SDL_FALSE);
		return std::nullopt;
	}

	std::optional<std::string> FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo((SDL_Window*)Application::Get().getWindow().getNativeWindow(), &wmInfo);
		ofn.hwndOwner = wmInfo.info.win.window;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrDefExt = std::strchr(filter, '\0') + 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		SDL_CaptureMouse(SDL_TRUE);
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			SDL_CaptureMouse(SDL_FALSE);
			return ofn.lpstrFile;
		}
		SDL_CaptureMouse(SDL_FALSE);
		return std::nullopt;
	}
#endif
}