#include "Lpch.h"
#include "PanelManager.h"

#include <imgui.h>

#include "Panels/ScenePanel.h"

namespace Laby {

	PanelManager::PanelEntry* PanelManager::Find(std::string_view key)
	{
		return &(*std::ranges::find_if(sEditorPanels, [key](const auto& panel) { return panel.key == key; }));
	}

	bool PanelManager::Contains(std::string_view key)
	{
		return std::ranges::find_if(sEditorPanels, [&](const PanelEntry& entry) { return entry.key == key; }) != sEditorPanels.end();
	}

	void PanelManager::Delete(std::string_view key)
	{
		if (!Contains(key))
		{
			LAB_ERROR("EditorPanel was not registered with manager");
			return;
		}

		std::erase_if(sEditorPanels, [&](const PanelEntry& entry) { return entry.key == key; });
	}

	void PanelManager::Render()
	{
		for (PanelEntry& panelEntry : sEditorPanels | std::views::filter([](const PanelEntry& entry) { return entry.panel && entry.displayed; }))
		{
			ImGui::Begin(panelEntry.key.data());
			panelEntry.panel->onImGuiRender();
			ImGui::End();
		}
	}

	void PanelManager::ProjectChanged()
	{
		for (PanelEntry& panelEntry : sEditorPanels | std::views::filter([](const PanelEntry& entry) { return entry.panel; }))
			panelEntry.panel->onProjectChange();
	}

	void PanelManager::SelectionChange()
	{
		for (PanelEntry& panelEntry : sEditorPanels | std::views::filter([](const PanelEntry& entry) { return entry.panel; }))
			panelEntry.panel->onSelectionChange();
	}
}