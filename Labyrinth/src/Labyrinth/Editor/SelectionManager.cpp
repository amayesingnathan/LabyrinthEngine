#include "Lpch.h"
#include "SelectionManager.h"

#include "PanelManager.h"

namespace Laby {

    void SelectionManager::Select(SelectionDomain domain, UUID selection)
    {
        if (IsSelected(domain, selection))
            return;

        sSelectionMap[domain].emplace_back(selection);
        PanelManager::SelectionChange();
    }

    void SelectionManager::Deselect(UUID selection)
    {
        for (auto& [domain, selections] : sSelectionMap)
        {
            auto it = std::find(selections.begin(), selections.end(), selection);
            if (it != selections.end())
            {
                selections.erase(it);
                PanelManager::SelectionChange();
            }
        }
    }
    void SelectionManager::Deselect(SelectionDomain domain, UUID selection)
    {
        if (sSelectionMap.contains(domain))
            return;

        auto& selections = sSelectionMap[domain];
        auto it = std::find(selections.begin(), selections.end(), selection);
        if (it != selections.end())
        {
            selections.erase(it);
            PanelManager::SelectionChange();
        }
    }

    void SelectionManager::DeselectAll()
    {
        sSelectionMap.clear();
        PanelManager::SelectionChange();
    }
    void SelectionManager::DeselectAll(SelectionDomain domain)
    {
        if (!sSelectionMap.contains(domain))
            return;

        sSelectionMap[domain].clear();
        PanelManager::SelectionChange();
    }

    bool SelectionManager::IsSelected(UUID selection)
    {
        for (const auto& [domain, selections] : sSelectionMap)
        {
            if (std::find(selections.begin(), selections.end(), selection) != selections.end())
                return true;
        }

        return false;
    }
    bool SelectionManager::IsSelected(SelectionDomain domain, UUID selection)
    {
        if (!sSelectionMap.contains(domain))
            return false;

        const auto& selections = sSelectionMap[domain];
        return std::find(selections.begin(), selections.end(), selection) != selections.end();
    }

    UUID SelectionManager::GetSelection(SelectionDomain domain, usize index)
    {
        if (!sSelectionMap.contains(domain))
            return 0;

        const auto& selections = sSelectionMap[domain];
        LAB_CORE_ASSERT(index < selections.size());

        return selections[index];
    }
    usize SelectionManager::GetSelectionCount(SelectionDomain domain)
    {
        if (!sSelectionMap.contains(domain))
            return 0;

        return sSelectionMap[domain].size();
    }

    const std::vector<UUID>& SelectionManager::GetSelections(SelectionDomain domain)
    {
        return sSelectionMap[domain];
    }

    void SelectionManager::ForEach(SelectionDomain domain, Action<UUID>&& func)
    {
        if (!sSelectionMap.contains(domain))
            return;

        for (UUID id : sSelectionMap[domain])
            func(id);
    }

    void SelectionManager::ForEach(Action<UUID>&& func)
    {
        for (const auto& [domain, selections] : sSelectionMap)
        {
            for (UUID id : selections)
                func(id);
        }
    }
}