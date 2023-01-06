#include "Lpch.h"
#include "IEditorModal.h"

#include <imgui.h>

#include <Labyrinth/Core/Application.h>

namespace Laby {

	IEditorModal::IEditorModal()
	{
		Application::BlockEsc();
	}

	IEditorModal::~IEditorModal()
	{
		Application::BlockEsc(false);
	}

	void WarningModal::onImGuiRender()
	{
		ImGui::Text(mMessage.c_str());
	}
}