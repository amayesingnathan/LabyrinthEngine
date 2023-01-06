#include "Lpch.h"
#include "PanelManager.h"

namespace Laby {

	void PanelManager::Delete(std::string_view key)
	{
		auto panel = Find(key);
		if (panel == sEditorPanels.end())
		{
			LAB_ERROR("EditorPanel was not registered with manager");
			return;
		}
		sEditorPanels.erase(panel);
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
		for (PanelEntry& PanelEntry : sEditorPanels)
		{
			if (PanelEntry.panel && PanelEntry.displayed) 
				PanelEntry.panel->onImGuiRender();
		}
	}

	void PanelManager::DispatchEvents(Event& e)
	{
		for (PanelEntry& PanelEntry : sEditorPanels)
		{
			if (PanelEntry.panel) 
				PanelEntry.panel->onEvent(e);
		}
	}

	void PanelManager::ProjectChanged(Ref<Project> project)
	{
		for (PanelEntry& PanelEntry : sEditorPanels)
		{
			if (PanelEntry.panel) 
				PanelEntry.panel->onProjectChange(project);
		}
	}

	void PanelManager::SelectionChange()
	{
		for (PanelEntry& PanelEntry : sEditorPanels)
		{
			if (PanelEntry.panel) 
				PanelEntry.panel->onSelectionChange();
		}
	}
}