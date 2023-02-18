#include "Lpch.h"
#include "OptionsPanel.h"

#include <Labyrinth/ImGui/ImGuiCpp.h>
#include <Labyrinth/Scripting/ScriptEngine.h>

using imcpp::Widgets;

namespace Laby {

	void OptionsPanel::onImGuiRender()
	{
		if (!mEditorData)
			return;

		Widgets::Checkbox("Display Colliders", mEditorData->displayColliders);
		Widgets::ColourEdit("Collider Colour", mEditorData->colliderColour);
		Widgets::ColourEdit("Selection Colour", mEditorData->selectionColour);
		Widgets::Button("Reset Camera Position", [this]() { mEditorData->camera->resetPosition(); });
		Widgets::Button("Reset Camera Angle", [this]() { mEditorData->camera->resetAngle(); });
		Widgets::Button("Reset Camera Angle", [this]() { mEditorData->camera->resetAngle(); });
		Widgets::Button("Regenerate Script Project", ScriptEngine::RegenScriptProject);
	}
}