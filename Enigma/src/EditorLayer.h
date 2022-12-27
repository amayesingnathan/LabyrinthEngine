#pragma once

#include "Labyrinth/Core/Application.h"

namespace Laby {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		virtual void onEvent(Event& e);
	};
}