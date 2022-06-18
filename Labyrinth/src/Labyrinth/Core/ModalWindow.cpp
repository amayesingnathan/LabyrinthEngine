#include "Lpch.h"
#include "ModalWindow.h"

#include <Labyrinth/Core/Application.h>

#include <imgui.h>

namespace Labyrinth {

	ModalWindow::ModalWindow(void* inData) : mPayload(inData) { Application::BlockEsc(); }

	void ModalWindow::Close()
	{
		mComplete = true;

		ImGui::CloseCurrentPopup();
		Application::BlockEsc(false);
	};

}