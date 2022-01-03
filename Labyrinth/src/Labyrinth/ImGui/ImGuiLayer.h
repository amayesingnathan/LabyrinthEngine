#pragma once

#include "Labyrinth/Core/Layer.h"

#include "Labyrinth/Events/ApplicationEvent.h"
#include "Labyrinth/Events/MouseEvent.h"
#include "Labyrinth/Events/KeyEvent.h"

namespace Labyrinth {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onUpdate() override;
		void onEvent(Event& e) override;

		void BlockEvents(bool block) { mBlockEvents = block; }

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

	private:
		bool mBlockEvents = true;
		int mTime = 0;
	};

}

