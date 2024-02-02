#include "Lpch.h"
#include "OptionsPanel.h"

#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Scripting/ScriptEngine.h>

namespace Laby {

	void OptionsPanel::OnRender()
	{
		if (!mEditorData)
			return;

		Widgets::Checkbox("Display Colliders", mEditorData->displayColliders);
		Widgets::ColourEdit("Collider Colour", mEditorData->colliderColour);
		Widgets::ColourEdit("Selection Colour", mEditorData->selectionColour);
		Widgets::Button("Reset Camera Position", [this]() { mEditorData->camera->ResetPosition(); });
		Widgets::Button("Reset Camera Angle", [this]() { mEditorData->camera->ResetAngle(); });
		Widgets::Button("Reset Camera Angle", [this]() { mEditorData->camera->ResetAngle(); });
		Widgets::Button("Regenerate Script Project", ScriptEngine::RegenScriptProject);
	}
}