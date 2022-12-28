#include "EditorLayer.h"

namespace Laby {

	EditorLayer::EditorLayer()
	{
	}

	void EditorLayer::onAttach()
	{
	}

	void EditorLayer::onDetach()
	{
	}

	void EditorLayer::onUpdate(Timestep ts)
	{
	}

	void EditorLayer::onImGuiRender()
	{
	}

	void EditorLayer::onEvent(Event& e)
	{
		LocalEventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(OnKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		return false;
	}
}