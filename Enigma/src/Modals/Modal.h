#pragma once

#include <Labyrinth/Core/Application.h>

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

	private:
		void display()
		{
			onImGuiRender();

			if (ImGui::IsKeyPressed(Key::Escape))
				Close();
		}

		friend class ModalManager;


	protected:
		bool mComplete = false;
	};
}