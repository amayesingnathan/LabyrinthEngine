#include "Lpch.h"
#include "IEditorModal.h"

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/ImGui/ImGuiCpp.h>

using imcpp::Widgets;

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
		Widgets::Label(mMessage);
	}
}