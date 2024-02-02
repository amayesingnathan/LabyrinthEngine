#include "Lpch.h"
#include "PanelManager.h"

#include <Labyrinth/ImGui/ImGuiWidgets.h>

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
			Widgets::BeginWindow(panelEntry.key);
			panelEntry.panel->OnRender();
			Widgets::EndWindow();
		}
	}

	void PanelManager::ProjectChanged()
	{
		for (PanelEntry& panelEntry : sEditorPanels | std::views::filter([](const PanelEntry& entry) { return entry.panel; }))
			panelEntry.panel->OnProjectChange();
	}

	void PanelManager::SelectionChange()
	{
		for (PanelEntry& panelEntry : sEditorPanels | std::views::filter([](const PanelEntry& entry) { return entry.panel; }))
			panelEntry.panel->OnSelectionChange();
	}
}