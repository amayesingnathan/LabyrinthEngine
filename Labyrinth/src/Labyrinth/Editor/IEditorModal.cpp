#include "Lpch.h"
#include "IEditorModal.h"

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

namespace Laby {

	IEditorModal::IEditorModal()
	{
		Application::BlockEsc();
	}

	IEditorModal::~IEditorModal()
	{
		Application::BlockEsc(false);
	}

	void WarningModal::OnRender()
	{
		Widgets::Label(mMessage);
	}
}