#include "Lpch.h"
#include "ImGuiHandler.h"

#include <Labyrinth/Core/Application.h>

namespace Laby {

	ImGuiHandler::ImGuiHandler()
		: imcpp::ImHandler
		(
			[]() { return Application::Get().getWindow().getNativeWindow(); },
			[]() { return imcpp::Utils::ToImVec<ImVec2>(Application::Get().getWindow().getSize()); }
		)
	{
	}

	void ImGuiHandler::onEvent(Event& e)
	{
		if (!mBlockEvents)
			return;

		ImGuiIO& io = ImGui::GetIO();
		e.handled |= (e.type & EventType::EVENT_CATEGORY_MOUSE) && io.WantCaptureMouse;
		e.handled |= (e.type & EventType::EVENT_CATEGORY_KEY) && io.WantCaptureKeyboard;
	}
}