#pragma once

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/Project/Project.h>

#include <imgui/imgui.h>

namespace Labyrinth
{
	class Modal : public RefCounted
	{
	public:
		Modal() { Application::BlockEsc(); }

		virtual void onImGuiRender() = 0;

		bool complete() { return mComplete; }

	protected:
		virtual void Close()
		{
			mComplete = true;

			ImGui::CloseCurrentPopup();
			Application::BlockEsc(false);
		};

	protected:
		bool mComplete = false;

		friend class ModalManager;
	};
}