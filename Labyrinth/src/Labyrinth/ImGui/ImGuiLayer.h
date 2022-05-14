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
		~ImGuiLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onEvent(Event& event) override;

		void begin();
		void end();

		void blockEvents(bool block) { mBlockEvents = block; }

		void SetDarkThemeColours();

	private:
		bool mBlockEvents = true;
	};

}

