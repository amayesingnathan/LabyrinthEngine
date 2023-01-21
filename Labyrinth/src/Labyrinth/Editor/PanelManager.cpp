#include "Lpch.h"
#include "PanelManager.h"

#include <imgui.h>

#include "Panels/ScenePanel.h"

namespace Laby {

	PanelManager::PanelEntry* PanelManager::Find(std::string_view key)
	{
		if (!Contains(key))
			return nullptr;

		return &sEditorPanels[sPanelIndices.at(key)];
	}

	void PanelManager::Delete(std::string_view key)
	{
		if (!Contains(key))
		{
			LAB_ERROR("EditorPanel was not registered with manager");
			return;
		}

		if (sPanelIndices.at(key) == sEditorPanels.size() - 1)
		{
			sEditorPanels.pop_back();
			sPanelIndices.erase(key);
			return;
		}

		sEditorPanels.erase(sEditorPanels.begin() + sPanelIndices.at(key));

		sPanelIndices.clear();
		for (usize i = 0; i < sEditorPanels.size(); i++)
			sPanelIndices[sEditorPanels[i].key] = i;
	}

	void PanelManager::Update(Timestep ts)
	{
		for (PanelEntry& PanelEntry : sEditorPanels)
		{
			if (PanelEntry.panel) 
				PanelEntry.panel->onUpdate(ts);
		}
	}

	void PanelManager::Render()
	{
		for (PanelEntry& panelEntry : sEditorPanels)
		{
			ImGui::Begin(panelEntry.key.data());
			if (panelEntry.panel && panelEntry.displayed)
				panelEntry.panel->onImGuiRender();
			ImGui::End();
		}
	}

	void PanelManager::ProjectChanged(Ref<Project> project)
	{
		for (PanelEntry& panelEntry : sEditorPanels)
		{
			if (panelEntry.panel)
				panelEntry.panel->onProjectChange(project);
		}
	}

	void PanelManager::SelectionChange()
	{
		for (PanelEntry& panelEntry : sEditorPanels)
		{
			if (panelEntry.panel)
				panelEntry.panel->onSelectionChange();
		}
	}
}