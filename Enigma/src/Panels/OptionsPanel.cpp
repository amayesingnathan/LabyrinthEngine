#include "OptionsPanel.h"

#include "../EditorLayer.h"

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/Scripting/ScriptEngine.h>

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	void OptionsPanel::onImGuiRender()
	{
		if (!mEditorData) return;

		ImGui::Begin("Settings");

		ImGui::Checkbox("Display Colliders", &mEditorData->displayColliders);
		ImGui::ColorEdit4("Collider Colour", glm::value_ptr(mEditorData->colliderColour));
		ImGui::ColorEdit4("Selection Colour", glm::value_ptr(mEditorData->selectionColour));
		if (ImGui::Button("Reset Camera Angle"))
			mEditorData->camera.resetAngle();
		if (ImGui::Button("Reset Camera Position"))
			mEditorData->camera.resetPosition();
		ImGui::Checkbox("Link Children On Destroy", &mEditorData->linkOnDestroy);
		if (ImGui::Button("Reload Scripts"))
			ScriptEngine::ReloadAssembly(Application::Get().getSpec().scriptConfig.appAssemblyPath);

		ImGui::End();
	}
}

